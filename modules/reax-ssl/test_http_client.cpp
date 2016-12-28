#include "http_client.hpp"

#include <mettle.hpp>
using namespace mettle;

using namespace reax;

class test_rw: public net::readwritable {
public:
  test_rw(reactor * r): r_(r){}

  virtual ~test_rw(){}
  future<std::size_t> write(const char * c, std::size_t size) override final {
    written_ += std::string{c, size};
    return r_->make_ready_future<std::size_t>(size);
  }

  future<std::size_t> read(char * c, std::size_t size) override final {
    if (size >= to_read_.size()) {
      std::size_t s = to_read_.size();
      strncpy(c, to_read_.c_str(), s);
      to_read_ = "";
      return r_->make_ready_future<std::size_t>(s);
    } else {
      auto it = to_read_.begin() + size;
      strncpy(c, to_read_.c_str(), size);
      to_read_ = std::string(it, to_read_.end());
      return r_->make_ready_future<std::size_t>(size);
    }
  }

  std::string to_read_;
  std::string written_;
  reactor * r_;
};

suite<> http_client("Simple tests for HTTP Client.", [](auto &_) {
  _.test("Simple request.", []() {
      http::response resp;
      std::unique_ptr<test_rw> skt;
      {
        executing_reactor e;
        skt = std::make_unique<test_rw>(&e.r);
        skt->to_read_ = "HTTP/1.1 200 OK\r\n"
          "Date: Mon, 27 Jul 2009 12:28:53 GMT\r\n"
          "Server: Apache/2.2.14 (Win32)\r\n"
          "Last-Modified: Wed, 22 Jul 2009 19:15:56 GMT\r\n"
          "Content-Length: 5\r\n"
          "Content-Type: text/html\r\n"
          "Connection: Closed\r\n"
          "\r\n"
          "ohhai";
        
        http::client c(&e.r);
        c.request_over_socket(
            "host.name",
            "/doggy",
            skt.get()
        ).then([&](auto r) {
          resp = r;
        });

      }
      expect(resp.status_code, equal_to(200));
      expect(resp.version, equal_to("1.1"));
      expect(resp.protocol, equal_to("HTTP"));
      expect(resp.status, equal_to("OK"));
      expect(resp.get_header("Content-Type", ""), equal_to("text/html"));
      expect(resp.get_header("Bad-header", "def"), equal_to("def"));
      expect(resp.content, equal_to("ohhai"));
      auto i1 = skt->written_.find('\r');
      std::string firstline = std::string(skt->written_.begin(), skt->written_.begin() + i1);
      expect(firstline, equal_to("GET /doggy HTTP/1.1"));
  });
});
