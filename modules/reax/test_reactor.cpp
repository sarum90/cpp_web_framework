#include "reactor.hpp"

#include <mettle.hpp>
using namespace mettle;

suite<> basic("a basic suite", [](auto &_) {

  _.test("a test", []() {
    expect(true, equal_to(true));
  });

  for(int i = 0; i < 4; i++) {
    _.test("test number " + std::to_string(i), [i]() {
      expect(i % 2, less(2));
    });
  }

  subsuite<>(_, "a subsuite", [](auto &_) {
    _.test("a sub-test", []() {
      expect(true, equal_to(true));
    });
  });

});

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
