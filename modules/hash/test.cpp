#include "mmh3.hpp"

#include <mettle.hpp>
using namespace mettle;

template<typename T>
auto in(T &&expected) {
  return make_matcher(
    std::forward<T>(expected),
    [](const auto &actual, const auto &expected) -> bool {
      auto x = expected.find(actual);
      return x != expected.end();
    }, "contained in "
  );
}

suite<> hash_test("Simple hash tests", [](auto &_) {

  _.test("mmh3", []() {
    auto m = hash::make_mmh3("kitty");
    auto m2 = hash::make_mmh3("kitty");
    auto m3 = hash::make_mmh3("dog");
    expect(m, equal_to(m2));
    expect(m, is_not(equal_to(m3)));
  });

  _.test("combine", []() {
    auto m = hash::make_mmh3("a");
    auto m2 = hash::make_mmh3("b");
    auto m3 = hash::make_mmh3("c");
    expect(m.combine_with(m), equal_to(m.combine_with(m)));
    expect(m.combine_with(m), is_not(equal_to(m.combine_with(m2))));
    expect(m.combine_with(m2), equal_to(m.combine_with(m2)));
    expect(m.combine_with(m), is_not(equal_to(m2.combine_with(m2))));
    expect(m.combine_with(m2), is_not(equal_to(m2.combine_with(m))));
    expect(hash::combine_mmh3({m, m2, m3}), is_not(equal_to(hash::combine_mmh3({m, m3, m2}))));
  });

  _.test("unordered combine", []() {
    auto m = hash::make_mmh3("a");
    auto m2 = hash::make_mmh3("b");
    auto m3 = hash::make_mmh3("c");
    expect(m.unordered_combine_with(m), equal_to(m.unordered_combine_with(m)));
    expect(m.unordered_combine_with(m), is_not(equal_to(m.unordered_combine_with(m2))));
    expect(m.unordered_combine_with(m2), equal_to(m.unordered_combine_with(m2)));
    expect(m.unordered_combine_with(m), is_not(equal_to(m2.unordered_combine_with(m2))));
    expect(m.unordered_combine_with(m2), equal_to(m2.unordered_combine_with(m)));
    expect(hash::unordered_combine_mmh3({m, m2, m3}), equal_to(hash::unordered_combine_mmh3({m, m3, m2})));
    std::deque<hash::mmh3> d{m, m2, m3};
    std::deque<hash::mmh3> d2{m3, m2, m};
    expect(hash::unordered_combine_mmh3(d), equal_to(hash::unordered_combine_mmh3(d2)));

    std::vector<hash::mmh3> v;
    std::set<hash::mmh3> s;
    for (int i = 0; i < 512; i++) {
      auto h = hash::unordered_combine_mmh3(v);
      expect(h, is_not(in(s)));
      s.insert(h);
      v.push_back(m);
    }
  });
});
