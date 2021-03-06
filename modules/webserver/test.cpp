#include "webserver.hpp"

using namespace webserver;

auto setup_server(reactor* r) {
  auto pr = r->make_promise<>();
  auto f = pr.get_future();

  auto s = std::make_unique<server>(create_server(r, [pr=std::move(pr)](auto& _) constexpr mutable {
    _.addRoute("/hw", [](auto& req, auto& resp) constexpr {
      return plaintext_response(resp, "Hello World");
    });

    _.addRoute("/file", [](auto& req, auto& resp) constexpr {
      return file_response(resp, "test.txt");
    });

    _.addRoute("/client.js", [](auto& req, auto& resp) constexpr {
      return file_response(resp, "client.js");
    });

    _.addRoute("/", [](auto& req, auto& resp) constexpr {
      return file_response(resp, "index.html");
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
  executing_reactor r;
  setup_server(&r.r);
}
