#include "webserver.hpp"

using namespace webserver;

auto setup_server(reactor* r) {
  promise<> pr;
  auto f = pr.get_future();
  auto s = std::make_unique<server>(create_server([pr=std::move(pr)](auto& _) constexpr mutable {
    _.addRoute("/", [](auto& req, auto& resp) constexpr {
      return plaintext_response(resp, "Hello World");
    });

    auto l = [pr=std::move(pr)](auto& req, auto& resp) constexpr mutable {
      pr.set_value();
      return plaintext_response(resp, "Killing!");
    };

    auto s = std::make_shared<decltype(l)>(std::move(l));

    _.addRoute("/terminate", [s=s](auto& req, auto& resp) {
      return (*s)(req, resp);
    });
  }));

  register_server(*s, r);
  f.then([s=s.get()]() {
    std::cout << "Concluding the server." << std::endl;
    return s->finish_outstanding();
  }).then([s=std::move(s)]() {
    std::cout << "Deleting the server." << std::endl;
  });
}

int main(int argc, char ** argv) {
  reactor r;
  reactor_setter rs(&r);
  setup_server(&r);
  r.run();
}
