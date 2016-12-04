#include "webserver.hpp"

using namespace webserver;

auto setup_server(reactor* r) {
  auto s = create_server([](auto& _) constexpr {
    _.addRoute("/", [](auto& req, auto& resp) constexpr {
      return plaintext_response(resp, "Hello World");
    });
  });
  register_server(s, r);
  return s;
}

int main(int argc, char ** argv) {
  reactor r;
  reactor_setter rs(&r);
  auto x = setup_server(&r);
  r.run();
}
