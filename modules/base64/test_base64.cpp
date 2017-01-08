#include <string>

#include "base64.hpp"
#include "mestring/parse.hpp"

#include <mettle.hpp>
using namespace mes;
using namespace mettle;

suite<> base64_basic("base64", [](auto &_) {

  _.test("Equality with standard strings.", []() {
    expect(base64_encode("{\"alg\":\"RS256\",\"typ\":\"JWT\"}"),
           equal_to("eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9"));

    expect(base64_encode("FFFF"), equal_to("RkZGRg=="));

    expect(base64_encode("FFFFF"), equal_to("RkZGRkY="));
    expect(base64_encode("FFFFFF"), equal_to("RkZGRkZG"));
    expect(base64_encode("FFFFFFF"), equal_to(std::string("RkZGRkZGRg==")));
    expect(base64_encode("FFFFFFFF"), equal_to(std::string("RkZGRkZGRkY=")));
    expect(base64_encode("FFFFFFFFF"), equal_to(std::string("RkZGRkZGRkZG")));
    expect(base64_encode("\xfb\xff"), equal_to("-_8="));
  });

  _.test("Size of the string", []() {
    auto be = base64_encode("");
    expect(be, equal_to(""));
    expect(be.size(), equal_to(0));

    for (auto txt : std::vector<mes::mestring>{
        "F", "FF", "FFF", "FFFF", "FFFFF", "FFFFFF", "FFFFFFF"}) {
      auto be = base64_encode(txt);
      expect(be.size(), equal_to(std::string(be).size()));
    }
  });

  _.test("NoEq Equality with standard strings.", []() {
    expect(base64_encode_noeq("{\"alg\":\"RS256\",\"typ\":\"JWT\"}"),
           equal_to("eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9"));

    expect(base64_encode_noeq("FFFF"), equal_to("RkZGRg"));

    expect(base64_encode_noeq("FFFFF"), equal_to("RkZGRkY"));
    expect(base64_encode_noeq("FFFFFF"), equal_to("RkZGRkZG"));
    expect(base64_encode_noeq("FFFFFFF"), equal_to(std::string("RkZGRkZGRg")));
    expect(base64_encode_noeq("FFFFFFFF"), equal_to(std::string("RkZGRkZGRkY")));
    expect(base64_encode_noeq("FFFFFFFFF"), equal_to(std::string("RkZGRkZGRkZG")));
    expect(base64_encode_noeq("\xfb\xff"), equal_to("-_8"));
  });

  _.test("NoEq Size of the string", []() {
    auto be = base64_encode_noeq("");
    expect(be, equal_to(""));
    expect(be.size(), equal_to(0));

    for (auto txt : std::vector<mes::mestring>{
        "F", "FF", "FFF", "FFFF", "FFFFF", "FFFFFF", "FFFFFFF"}) {
      auto be = base64_encode_noeq(txt);
      expect(be.size(), equal_to(std::string(be).size()));
    }
  });

  _.test("Reverse base64.", []() {
    expect(base64_decode("RkZGRg=="), equal_to("FFFF"));
    expect(base64_decode("RkZGRg"), equal_to("FFFF"));
    expect(base64_decode("RkZGRkY="), equal_to("FFFFF"));
    expect(base64_decode("RkZGRkZG"), equal_to("FFFFFF"));
    expect(base64_decode("RkZGRkZGRg=="), equal_to(std::string("FFFFFFF")));
    expect(base64_decode("RkZGRkZGRkY="), equal_to("FFFFFFFF"));
    expect(base64_decode("RkZGRkZGRkY"), equal_to("FFFFFFFF"));
    expect(base64_decode("RkZGRkZGRkZG"), equal_to("FFFFFFFFF"));
    expect(base64_decode("-_8="), equal_to("\xfb\xff"));
    expect(base64_decode("RkZGRkZ\rGR\nkZ\tG  "), equal_to("FFFFFFFFF"));
  });

});
