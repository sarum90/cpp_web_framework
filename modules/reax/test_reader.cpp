#include "reader.hpp"

#include <mettle.hpp>
using namespace mettle;

suite<> reactor("basic tests for reactor", [](auto &_) {
  _.test("execute later", []() {
      int i = 0;
      {
        executing_reactor e;
        schedule(e.r, [&i](){i = 1;});
      }
      expect(i, equal_to(1));
  });
});
