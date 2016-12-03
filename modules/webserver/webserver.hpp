#pragma once 

#include "reactor.hpp"

#include <iostream>
#include <functional>
#include <map>

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
      routes_.insert(std::make_pair(s, r));
    }

  private:
    std::map<std::string, route> routes_;
};

class connection {
  public:
    connection(std::unique_ptr<boost::asio::ip::tcp::socket> socket):
      socket_(std::move(socket)){}

    void load() {
      read_more();
    }

  private:

    void read_more() {
      socket_->async_read_some(
        boost::asio::buffer(buffer_),
        [this](const boost::system::error_code& ec, size_t bytes) {
          return receive_data(ec, bytes);
        }
      );
    }

    void receive_data(const boost::system::error_code& ec, size_t bytes) {
      request_content_ += std::string{buffer_.begin(), bytes};
      process_content();
    }

    void process_content() {
      std::cout << request_content_ << std::endl;
    }

    std::unique_ptr<boost::asio::ip::tcp::socket> socket_;
    std::string request_content_;
    std::array<char, 4096> buffer_;
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

  private:
    template <class T>
    void handle_connection(T&& t) {
      t.then([](auto socket) {
        connection* c = new connection{std::move(socket)};
        c->load();
      });
    }

		void do_accept(reactor * r) {
      auto socket = std::make_unique<boost::asio::ip::tcp::socket>(r->io_service_);
      auto* sref = socket.get();
      promise<std::unique_ptr<boost::asio::ip::tcp::socket>> ret_promise;
      auto retval = ret_promise.get_future();

      auto x = [this, r=r, s=std::move(socket), p=std::move(ret_promise)](const boost::system::error_code& ec) mutable {
        if (ec) {
          p.set_exception(ec);
        } else {
          if (!shutting_down_) {
            do_accept(r);
          }
          p.set_value(std::move(s));
        }
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
