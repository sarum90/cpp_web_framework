
#include "reactor.hpp"

#include "http_client.hpp"

void setup_run(reactor * r) {
  http::get(r, "http://localhost:8000/test.txt").then([](http::response res) {
    std::cout << res.protocol << std::endl;
    for (auto h : res.headers) {
      std::cout << h.first << ":" << h.second << std::endl;
    }
    std::cout << "======" << std::endl;
    std::cout << res.content;
    std::cout << "======" << std::endl;
    std::cout << res.content.size() << std::endl;
  });
}

int main(int argc, char ** argv) {
  executing_reactor r;
  setup_run(&r.r);
}
