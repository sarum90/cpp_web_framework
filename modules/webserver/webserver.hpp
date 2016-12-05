#pragma once 

#include "reactor.hpp"

#include "future/do_with.hh"

#include <iostream>
#include <functional>
#include <map>
#include <sstream>

namespace webserver {

namespace {

struct request { };
struct response { };
struct return_type{
  std::string str;
};

class router {
  private:
    using route = std::function<return_type(request&, response&)>;

  public:
    void addRoute(const std::string& s, route r) {
      routes_.insert(std::make_pair(s, std::move(r)));
    }

    route& getRoute(std::string& s) {
      return routes_[s];
    }

  private:
    std::map<std::string, route> routes_;
};

class connection {
  public:
    connection(std::unique_ptr<boost::asio::ip::tcp::socket> socket):
      socket_(std::move(socket)){}

    future<std::string, std::string> get_info() {
      read_more();
      return result_.get_future();
    }

    auto send_response(const std::string& s) {
      buffer_to_send_ = s;
      send_offset_ = 0;
      send_as_much_as_possible();
      return send_promise_.get_future();
    }

  private:

    void send_as_much_as_possible() {
      socket_->async_write_some(
          boost::asio::buffer(&buffer_to_send_[send_offset_], buffer_to_send_.size() - send_offset_),
          [this](const boost::system::error_code& ec, size_t bytes) {
            sent_data(ec, bytes);
          }
      );
    }

    void sent_data(const boost::system::error_code& ec, size_t bytes) {
      if (ec) {
        send_promise_.set_exception(ec);
        return;
      }
      send_offset_ += bytes;
      if (send_offset_ == buffer_to_send_.size()) {
        send_promise_.set_value();
      } else {
        send_as_much_as_possible();
      }
    }

    void read_more() {
      socket_->async_read_some(
        boost::asio::buffer(buffer_),
        [this](const boost::system::error_code& ec, size_t bytes) {
          return receive_data(ec, bytes);
        }
      );
    }

    void receive_data(const boost::system::error_code& ec, size_t bytes) {
      if (ec) {
        result_.set_exception(ec);
        return;
      }
      request_content_ += std::string{buffer_.begin(), bytes};
      process_content();
    }

    void process_content() {
      auto pos = request_content_.find("\r\n\r\n");
      if (pos == std::string::npos) {
        std::cout << "Need to get more! UNTESTED!" << std::endl;
        // TODO: Test.
        read_more();
      } else {
        parse_content(request_content_);
      }
    }

    void parse_content(const std::string& s) {
      std::istringstream iss(s);
      std::string method, path;
      iss >> method >> path;
      result_.set_value(method, path);
    }

    std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
    std::string request_content_;
    std::array<char, 4096> buffer_;
    std::string buffer_to_send_;
    int send_offset_;
    promise<std::string, std::string> result_;
    promise<> send_promise_;
};

class server {
  public:
    class router& router() {return router_;}

		void start_listening(
				reactor* r,
				const boost::asio::ip::tcp::resolver::endpoint_type& endpoint) {
			acceptor_ = std::make_unique<boost::asio::ip::tcp::acceptor>(r->io_service_);
			acceptor_->open(endpoint.protocol());
			acceptor_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acceptor_->bind(endpoint);
			acceptor_->listen();
			std::cout << "Listening, waiting for connections on : http://"
								<< acceptor_->local_endpoint() << std::endl;
      do_accept(r);
		}

    future<> finish_outstanding() {
      shutting_down_ = true;
      acceptor_->close();
      if (outstanding_requests_ == 0) {
        outstanding_finished_.set_value();
      }
      return outstanding_finished_.get_future();
    }

  private:
    template <class T>
    void handle_connection(T&& t) {
      if (shutting_down_) {
        return;
      }
      outstanding_requests_++;
      do_with(
          std::unique_ptr<connection>{nullptr},
          [t=std::move(t), this](std::unique_ptr<connection>& c) mutable {
            return t.then([&c](auto socket) {
              c.reset(new connection{std::move(socket)});
              return c->get_info();
            }).then([&c, this](std::string method, std::string path) mutable {
              std::cout << method << " : " << path<< std::endl;
              auto r = router_.getRoute(path);
              if (r) {
                request req{};
                response res{};
                return c->send_response(
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: text/plain; charset=us-ascii\r\n"
                  "\r\n" + r(req, res).str
                );
              } else {
                return c->send_response(
                  "HTTP/1.1 404 Not Found\r\n"
                  "Content-Type: text/plain; charset=us-ascii\r\n"
                  "\r\n"
                  "Endpoint not found."
                );
              }
            });
          }
      ).then([this]() {
        outstanding_requests_--;
        if (shutting_down_ && outstanding_requests_ == 0) {
          outstanding_finished_.set_value();
        }
      });
    }

		void do_accept(reactor * r) {
      auto socket = std::make_unique<boost::asio::ip::tcp::socket>(r->io_service_);
      auto* sref = socket.get();
      promise<std::unique_ptr<boost::asio::ip::tcp::socket>> ret_promise;
      auto retval = ret_promise.get_future();

      auto x = [this, r=r, s=std::move(socket), p=std::move(ret_promise)](const boost::system::error_code& ec) mutable {
        if (shutting_down_) {
          return;
        }
        if (ec) {
          std::cout << "Error! " << ec << std::endl;
          p.set_exception(ec);
          return;
        }
        do_accept(r);
        p.set_value(std::move(s));
      };
      auto handler = std::make_shared<decltype(x)>(std::move(x));

      acceptor_->async_accept(*sref, [h=std::move(handler)](const boost::system::error_code& ec) {
        return (*h)(ec);
      });

      handle_connection(retval);
		}

    class router router_;
		std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_ = nullptr;
    bool shutting_down_ = false;
    int outstanding_requests_ = 0;
    promise<> outstanding_finished_;
};

}

template <class L>
server create_server(L l) {
  server s;
  l(s.router());
  return s;
}

return_type plaintext_response(response& r, const std::string& str) {
  return {str};
}

void register_server(server& s, reactor* r) {
  net::resolve_tcp(r, {boost::asio::ip::address::from_string("127.0.0.1"), 0})
  .then([s=&s, r=r](auto iter) mutable {
		s->start_listening(r, iter->endpoint());
  });
}

}
