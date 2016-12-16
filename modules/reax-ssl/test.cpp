
#include "reactor.hpp"

#include "http_client.hpp"

void setup_run(reactor * r) {
  http_get(r, mes::make_mestring("http://localhost/test.txt")).then([](auto res) {
    std::cout << res.content << std::endl;
  });
}

int main(int argc, char ** argv) {
  executing_reactor r;
  setup_run(&r.r);
}
