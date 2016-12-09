
#include <iostream>


#include "emscripten/emscripten.h"

void printit(void * v) {
  std::cout << "Waited" << std::endl;
}

int main(int argc, char ** argv) {
  std::cout << "Hell o _ world" << std::endl;

  emscripten_async_call(printit, nullptr, 5000);
  return 0;
}
