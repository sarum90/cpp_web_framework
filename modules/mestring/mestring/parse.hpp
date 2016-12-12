#pragma once

#include <tuple>
#include <limits>

#include "../mestring.hpp"
#include "parse-impl.hpp"

namespace mes {

constexpr int parse_int(const mestring& me) {
  int retval = 0;
  bool neg = false;
  auto c = me.begin();

  constexpr int mx = std::numeric_limits<int>::max();
  constexpr int mxd10 = mx / 10;
  constexpr int mn = std::numeric_limits<int>::min();
  constexpr int mnd10 = mn / 10;
  if (*c == '-') {
    ++c;
    for (;c != me.end(); ++c) {
      int val = *c - '0';
      if (val < 0 || val > 9) {
        throw parse_error("Error parsing int.");
      }
      if (mnd10 > retval) {
        throw parse_error("Parse integer underflow.");
      }
      retval *= 10;
      if (mn + val > retval) {
        throw parse_error("Parse integer underflow.");
      }
      retval -= val;
    }
    return retval;
  } else {
    for (;c != me.end(); ++c) {
      int val = *c - '0';
      if (val < 0 || val > 9) {
        throw parse_error("Error parsing int.");
      }
      if (mxd10 < retval) {
        throw parse_error("Parse integer overflow.");
      }
      retval *= 10;
      if (mx - val < retval) {
        throw parse_error("Parse integer overflow.");
      }
      retval += val;
    }
    return retval;
  }
}

template <int n>
constexpr typename impl::static_split<n>::return_type static_split(const mestring& m, char c) {
  return impl::static_split<n>::call(m, c);
}

} // namespace mes
