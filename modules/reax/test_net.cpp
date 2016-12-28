#include "net.hpp"

#include "future/do_with.hh"

#include <mettle.hpp>
using namespace mettle;

suite<> endpoint("basic tests for reactor", [](auto &_) {
  _.test("Simple address parsing", []() {

      expect(net::ipv4_endpoint(0xff000000, 123),
             equal_to(net::ipv4_endpoint(mes::make_mestring("255.0.0.0:123"))));
      expect(net::ipv4_endpoint(0x7f000001, 8080),
             equal_to(net::ipv4_endpoint(mes::make_mestring("127.0.0.1:8080"))));
      static_assert(net::ipv4_endpoint(0, 123) == net::ipv4_endpoint(mes::make_mestring("0.0.0.0:123")));
  });
});

suite<> acceptor("basic tests creating an accepting socket", [](auto &_) {
  _.test("Accept locally", []() {
    {
      executing_reactor er;
      net::make_acceptor(er.get_reactor(), mes::make_mestring("127.0.0.1:0"))
        .then([r=er.get_reactor()](net::acceptor a) {
          auto acc = reax::make_concluding<net::acceptor>(std::move(a));

          net::connect_to(r, net::ipv4_endpoint(mes::make_mestring("127.0.0.1"), acc->ep().port()))
            .then([r=r](net::socket s) {
              return do_with(std::move(s), [r=r](net::socket &s) {
                return reax::readline(r, &s).then([](std::string str) {
                    expect(str, equal_to("Hello World\n"));
                });
              });
            });

          return acc->accept();
        }).then([r=er.get_reactor()](net::socket s) {
          return do_with(std::move(s), [r=r](net::socket& s) {
            return reax::write(r, &s, mes::make_mestring("Hello World\n"));
          });
        });
    }
  });
});
