#pragma once

#include "mestring.hpp"
#include "mestring/parse.hpp"
#include "future/future.hh"
#include <boost/asio.hpp>
#include "reactor.hpp"

#include <cassert>

namespace net {

class gate {
  public:
    gate(reactor * r): closed_(r->make_promise<>()) {}

    future<> close() {
      assert(!closing_);
      closing_ = true;
      if (count_ == 0) {
        closed_.set_value();
      }
      return closed_.get_future();
    }

    void inc() {
      count_++;
    }

    void dec() {
      assert(count_ > 0);
      count_--;
      if (count_ == 0 && closing_) {
        closed_.set_value();
      }
    }

  private:
    int count_ = 0;
    bool closing_ = false;
    promise<> closed_;
};

future<boost::asio::ip::tcp::resolver::iterator> resolve_tcp(
    reactor* r, const boost::asio::ip::tcp::resolver::endpoint_type& endpoint);

future<boost::asio::ip::tcp::resolver::iterator> resolve_tcp(
    reactor* r, const boost::asio::ip::tcp::resolver::query& endpoint);

class ipv4_endpoint;

constexpr inline ipv4_endpoint parse_ipv4(const mes::mestring& s);
constexpr inline unsigned long parse_addr(const mes::mestring& s);

class ipv4_endpoint {
  public:
    ipv4_endpoint(const boost::asio::ip::tcp::endpoint& ep):
      _address(ep.address().to_v4().to_ulong()), _port(ep.port()){}

    constexpr ipv4_endpoint(unsigned long addr, unsigned short p):
      _address(addr), _port(p){}

    constexpr ipv4_endpoint(const mes::mestring& str): ipv4_endpoint(parse_ipv4(str)){}

    constexpr ipv4_endpoint(const mes::mestring& str, unsigned short p):
      _address(parse_addr(str)), _port(p){}

    operator boost::asio::ip::tcp::endpoint() const {
      return boost::asio::ip::tcp::endpoint(
          boost::asio::ip::address_v4(_address), _port);
    }

    constexpr bool operator==(const ipv4_endpoint& other) const {
      return _address == other._address && _port == other._port;
    }

    unsigned short port() {
      return _port;
    }

  private:
    constexpr int p(int part) const {
      return 0xff & (_address >> ((3 -part) * 8));
    }

    unsigned long _address;
    unsigned short _port;
    friend std::ostream& operator<<(std::ostream& o, const ipv4_endpoint& e);
};

std::ostream& operator<<(std::ostream& o, const ipv4_endpoint& e) {
  return (o << e.p(0) << "."
            << e.p(1) << "."
            << e.p(2) << "."
            << e.p(3) << ":" << e._port);

}

constexpr inline unsigned long parse_addr(const mes::mestring& s) {
  auto addr_split = mes::static_split<4>(s, '.');
  unsigned int addr = 0;
  addr += mes::parse_int(std::get<0>(addr_split));
  addr <<= 8;
  addr += mes::parse_int(std::get<1>(addr_split));
  addr <<= 8;
  addr += mes::parse_int(std::get<2>(addr_split));
  addr <<= 8;
  addr += mes::parse_int(std::get<3>(addr_split));
  return addr;
}

constexpr inline ipv4_endpoint parse_ipv4(const mes::mestring& s) {
  auto split = mes::static_split<2>(s, ':');
  unsigned int addr = parse_addr(std::get<0>(split));
  unsigned short p = static_cast<unsigned short>(mes::parse_int(std::get<1>(split)));
  return ipv4_endpoint{addr, p};
}

class socket {
  public:
    socket(reactor * r): r_(r), asio_socket_(make_socket(r)){}

    socket(const socket& s) = delete;
    socket& operator=(const socket& s) = delete;

    socket(socket&& s) noexcept {
      *this = std::move(s);
    }
    socket& operator=(socket&& s) noexcept {
      r_ = s.r_; 
      asio_socket_ = std::move(s.asio_socket_);
      s.asio_socket_ = make_socket(r_);
      return *this;
    }

    // Retry writing until the whole string is written.
    future<std::size_t> write(const char * c, std::size_t size) {
      auto ret_promise = r_->make_promise<std::size_t>();
      auto ret_fut = ret_promise.get_future();

      auto x = [p=std::move(ret_promise)](const boost::system::error_code& ec, std::size_t bytes) mutable {
        if (ec) {
          p.set_exception(ec);
          return;
        }
        p.set_value(bytes);
      };
      auto handler = std::make_shared<decltype(x)>(std::move(x));

      asio_socket_->async_write_some(boost::asio::buffer(c, size),
          [h=std::move(handler)](const auto& ec, std::size_t b) {
          (*h)(ec, b);
      });

      return ret_fut;
    }

    future<std::size_t> read(char * c, std::size_t size) {
      auto ret_promise = r_->make_promise<std::size_t>();
      auto ret_fut = ret_promise.get_future();

      auto x = [p=std::move(ret_promise)](const boost::system::error_code& ec, std::size_t bytes) mutable {
        if (ec) {
          p.set_exception(ec);
          return;
        }
        p.set_value(bytes);
      };
      auto handler = std::make_shared<decltype(x)>(std::move(x));

      asio_socket_->async_read_some(boost::asio::buffer(c, size),
          [h=std::move(handler)](const auto& ec, std::size_t b) {
          (*h)(ec, b);
      });

      return ret_fut;
    }

  private:

    static std::unique_ptr<boost::asio::ip::tcp::socket> make_socket(reactor * r) {
      return std::make_unique<boost::asio::ip::tcp::socket>(r->io_service_);
    }

    reactor * r_;
    std::unique_ptr<boost::asio::ip::tcp::socket> asio_socket_;

    boost::asio::ip::tcp::socket* asio_socket() {
      return asio_socket_.get();
    }

    friend class acceptor;
    friend future<socket> connect_to(reactor * r, const ipv4_endpoint& mes);
};

class acceptor {
  public:
    unsigned short port;

    future<socket> accept() {
      auto ret_promise = r_->make_promise<socket>();
      auto retval = ret_promise.get_future();

      auto sock = next_.asio_socket();

      auto x = [g=&gate_, r=r_, s=std::move(next_), p=std::move(ret_promise)](const boost::system::error_code& ec) mutable {
        g->dec();
        if (ec) {
          p.set_exception(ec);
          return;
        }
        p.set_value(std::move(s));
      };
      auto handler = std::make_shared<decltype(x)>(std::move(x));

      gate_.inc();
      acceptor_->async_accept(*sock, [h=std::move(handler)](const boost::system::error_code& ec) {
        return (*h)(ec);
      });

      return retval;
    }

    future<> conclude() {
      return gate_.close();
    }

    ipv4_endpoint ep() const {
      return acceptor_->local_endpoint();
    }

 private:
    acceptor(std::unique_ptr<boost::asio::ip::tcp::acceptor> acc,
             reactor * r): acceptor_(std::move(acc)), r_(r), next_(r), gate_(r) {}

		std::unique_ptr<boost::asio::ip::tcp::acceptor> acceptor_;
    reactor * r_;
    socket next_;
    gate gate_;

    friend future<acceptor> make_acceptor(reactor * r, const ipv4_endpoint& ep);
};

future<acceptor> make_acceptor(reactor * r, const ipv4_endpoint& ep) {
			auto acc = std::make_unique<boost::asio::ip::tcp::acceptor>(r->io_service_);
      auto endpoint = boost::asio::ip::tcp::endpoint(ep);
			acc->open(endpoint.protocol());
			acc->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			acc->bind(endpoint);
			acc->listen();
      return r->make_ready_future<acceptor>(acceptor(std::move(acc), r));
}

future<socket> connect_to(reactor * r, const ipv4_endpoint& ep) {
  socket s(r);
  boost::system::error_code ec;
  s.asio_socket()->connect(ep, ec);
  if (ec) {
    return r->make_exception_future<socket>(std::make_exception_ptr(ec));
  }
  return r->make_ready_future<socket>(std::move(s));
}

}
