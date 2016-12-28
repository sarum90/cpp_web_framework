#include <string>

#include "mestring.hpp"
#include "mestring/parse.hpp"

#include <mettle.hpp>
using namespace mes;
using namespace mettle;

suite<> mestring_basic("mestring", [](auto &_) {

  _.test("Equality with standard strings.", []() {
    constexpr auto str = "Test_string";
    constexpr auto is_str = "Test_string";
    constexpr auto not_str = "not_Test_string";

    constexpr auto test_str = make_mestring(str);
    constexpr mestring test_str2("Test_string");
    expect(test_str, equal_to(test_str2));

    expect(test_str, equal_to(str));
    expect(test_str, equal_to(is_str));
    expect(test_str, is_not(equal_to(not_str)));
    expect(test_str == str, equal_to(true));
    expect(test_str != str, equal_to(false));
    expect(test_str == not_str, equal_to(false));
    expect(test_str != not_str, equal_to(true));

    expect(test_str, equal_to(std::string(str)));
    expect(test_str == std::string(str), equal_to(true));
    expect(test_str != std::string(str), equal_to(false));
    expect(test_str == std::string(not_str), equal_to(false));
    expect(test_str != std::string(not_str), equal_to(true));

    constexpr const char c[2] = {'a', 'b'};
    expect([&]() { return mestring{c}; },
           thrown<std::runtime_error>("Can only construct mestrings from string literals."));

    static_assert(test_str == str, "");
  });

  _.test("Empty string", []() {
      expect(mestring(""), equal_to(std::string{}));
  });

  _.test("Equality with self.", []() {
    constexpr auto str = "Test_string";
    constexpr auto str2 = "Test_string";
    constexpr auto str3 = "not_Test_string";

    constexpr auto test_str = make_mestring(str);
    constexpr auto test_str2 = make_mestring(str2);
    constexpr auto test_str3 = make_mestring(str3);

    expect(test_str, equal_to(test_str));
    expect(test_str, equal_to(test_str2));
    expect(test_str == test_str2, equal_to(true));
    expect(test_str != test_str2, equal_to(false));

    expect(test_str, is_not(equal_to(test_str3)));
    expect(test_str == test_str3, equal_to(false));
    expect(test_str != test_str3, equal_to(true));

    static_assert(test_str == test_str2, "");
    static_assert(test_str != test_str3, "");
  });

  _.test("Char accessor.", []() {
    constexpr auto str = "Test_string";
    constexpr auto test_str = make_mestring(str);

    expect(test_str[0], equal_to('T'));
    expect([&]() { return test_str[100]; },
           thrown<std::runtime_error>("Access off end of mestring."));
    expect([&]() { return test_str[-1]; },
           thrown<std::runtime_error>("Attempted negative mestring access."));

    static_assert (test_str[0] == 'T', "");
  });

  _.test("Size.", []() {
    constexpr auto test_str = make_mestring("12345");
    static_assert(test_str.size() == 5);
  });

  _.test("Iterate.", []() {
    constexpr auto test_str = make_mestring("12345");
    int i = 0;
    for (auto& c: test_str) {
      i++;
      expect(c - '0', equal_to(i));
    }
    expect(i, equal_to(5));
  });

  _.test("constexpr Iterate.", []() {
    constexpr auto test_str = make_mestring("12345");
    // There's probably a better way to make a constexpr lambda...
    constexpr struct {
      constexpr int operator ()(const mestring& x) const {
        int sum = 0;
        for(auto& c: x) {
          sum += c - '0';
        }
        return sum;
      }
    } l;
    static_assert(l(test_str) == 15);
  });

});

suite<> mestring_parsing("mestring parsing", [](auto &_) {

  _.test("parse int.", []() {
    expect(parse_int(make_mestring("123")), equal_to(123));
    expect(parse_int(make_mestring("-123")), equal_to(-123));
    expect(parse_int(make_mestring("-0")), equal_to(0));
    expect([](){return parse_int(make_mestring("DOG"));},
           thrown<parse_error>("Error parsing int."));
    expect([](){return parse_int(make_mestring("--"));},
           thrown<parse_error>("Error parsing int."));

    static_assert(parse_int(make_mestring("123")) == 123);
    static_assert(parse_int(make_mestring("-123")) == -123);
  });

  _.test("parse int overflow.", []() {
    constexpr int largest = std::numeric_limits<int>::max();
    constexpr int smallest = std::numeric_limits<int>::min();
    expect(parse_int(make_mestring(std::to_string(largest).c_str())), equal_to(largest));
    expect(parse_int(make_mestring(std::to_string(smallest).c_str())), equal_to(smallest));


    unsigned int too_large = largest;
    std::string way_too_large = "1" + std::string(100, '0');
    too_large += 1;
    expect([&]() {return parse_int(make_mestring(std::to_string(too_large).c_str()));},
           thrown<parse_error>("Parse integer overflow."));
    expect([&]() {return parse_int(make_mestring(way_too_large.c_str()));},
           thrown<parse_error>("Parse integer overflow."));

    auto s = std::to_string(smallest);
    std::string abssmallest(s.begin() + 1, s.end());
    unsigned int abs_smallest = std::stoul(abssmallest);
    abs_smallest += 1;
    std::string too_small = "-" + std::to_string(abs_smallest);
    std::string way_too_small = "-" + way_too_large;
    expect([&]() {return parse_int(make_mestring(too_small.c_str()));},
           thrown<parse_error>("Parse integer underflow."));
    expect([&]() {return parse_int(make_mestring(way_too_small.c_str()));},
           thrown<parse_error>("Parse integer underflow."));

  });

  _.test("simple static_split", []() {
    constexpr auto t = static_split<2>(make_mestring("12:23"), ':');
    expect(parse_int(std::get<0>(t)), equal_to(12));
    expect(parse_int(std::get<1>(t)), equal_to(23));

    static_assert(parse_int(std::get<0>(t)) == 12);
    static_assert(parse_int(std::get<1>(t)) == 23);

    expect([](){return static_split<2>(make_mestring("12"), ':');},
           thrown<parse_error>("Not enough separators for static_split."));

    expect([](){return static_split<2>(make_mestring(":12:"), ':');},
           thrown<parse_error>("Too many separators for static_split."));
  });

  _.test("simple static_split_first", []() {
    constexpr auto t = static_split_first<2>(make_mestring("12:23:::"), ':');
    expect(std::get<0>(t), equal_to("12"));
    expect(std::get<1>(t), equal_to("23:::"));
  });

  _.test("n-way static_split", []() {
    constexpr auto t = static_split<5>(make_mestring(":12:23:34:"), ':');
    expect(std::get<0>(t), equal_to(""));
    expect(parse_int(std::get<1>(t)), equal_to(12));
    expect(parse_int(std::get<2>(t)), equal_to(23));
    expect(parse_int(std::get<3>(t)), equal_to(34));
    expect(std::get<4>(t), equal_to(""));

    static_assert(std::get<0>(t) == "");
    static_assert(parse_int(std::get<1>(t)) == 12);
    static_assert(parse_int(std::get<2>(t)) == 23);
    static_assert(parse_int(std::get<3>(t)) == 34);
    static_assert(std::get<4>(t) == "");

    expect([](){return static_split<3>(make_mestring("12"), ':');},
           thrown<parse_error>("Not enough separators for static_split."));

    expect([](){return static_split<3>(make_mestring(":12"), ':');},
           thrown<parse_error>("Not enough separators for static_split."));

    expect([](){return static_split<5>(make_mestring(":12:::::::"), ':');},
           thrown<parse_error>("Too many separators for static_split."));
  });

});

suite<> mestring_cat_basic("mestring_cat", [](auto &_) {

  _.test("truncate", []() {
      {
        mestring_cat mc;
        mc += "a";
        mc += "b";
        mc += "c";
        mc += "dd";
        mc += "eee";
        mc += "f";
        mc += "g";
        int idx = find_index(mc, "dee");
        expect(idx, equal_to(4));
        expect(std::string(mestring_cat::truncated(std::move(mc), idx)), equal_to(std::string("abcd")));
      }

      {
        mestring_cat mc;
        mc += "abcde";
        expect(std::string(mestring_cat::truncated(std::move(mc), -1)), equal_to("abcd"));
      }
  });


  _.test("substring", []() {
        mestring_cat mc;
        mc += "abc";
        mc += "def";
        mc += "ghi";
        mc += "jkl";

        auto i = mc.begin();
        ++i;
        auto ii = i;
        ++ii;
        ++ii;
        ++ii;
        ++ii;
        ++ii;
        ++ii;
        expect(std::string(substr(i, ii)), equal_to("bcdefg"));
  });

  _.test("substring", []() {
        mestring_cat mc;
        mc += "do";
        mc += "g: c";
        mc += "at:cat";

        auto im = static_split_first<2>(mc, ':');
        auto key = std::move(std::get<0>(im));
        auto val = lstrip(std::move(std::get<1>(im)));
        expect(std::string(key), equal_to("dog"));
        expect(std::string(val), equal_to("cat:cat"));
  });

});
