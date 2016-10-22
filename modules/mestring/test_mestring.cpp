
#include <exception>
#include <string>

#include "mestring.hpp"


template<class A, class B>
void verify_equal(const A& a, const B& b) {
  if (a == b) {
    return;
  }
  throw std::exception();
}

int main(int argc, char ** argv) {
  constexpr auto str = "Test_string";

  constexpr auto test_str = make_mestring(str);

  verify_equal(test_str, str);
  verify_equal(test_str, std::string(str));

  static_assert(test_str == str, "");

  return 0;
}
