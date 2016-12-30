#pragma once

#include <tuple>
#include <limits>

#include "../mestring.hpp"
#include "parse-impl.hpp"

namespace mes {

template<class T>
decltype(auto) lstrip(T t) {
  auto i = t.begin();
  while ((*i == ' ' || *i == '\t') && i != t.end()) {
    ++i;
  }
  return substr(i, t.end());
}

template<class T, int base=10>
constexpr int parse_int(T me) {
  int retval = 0;
  auto c = me.begin();
  if (c == me.end()) {
    throw parse_error("Error parsing int.");
  }

  constexpr int mx = std::numeric_limits<int>::max();
  constexpr int mxd10 = mx / base;
  constexpr int mn = std::numeric_limits<int>::min();
  constexpr int mnd10 = mn / base;
  if (*c == '-') {
    ++c;
    if (c == me.end()) {
      throw parse_error("Error parsing int.");
    }
    for (;c != me.end(); ++c) {
      int val = *c - '0';
      if (*c >= 'a' && *c <= 'z') {
        val = *c - 'a' + 10;
      }
      if (*c >= 'A' && *c <= 'Z') {
        val = *c - 'A' + 10;
      }
      if (val < 0 || val > base-1) {
        throw parse_error("Error parsing int.");
      }
      if (mnd10 > retval) {
        throw parse_error("Parse integer underflow.");
      }
      retval *= base;
      if (mn + val > retval) {
        throw parse_error("Parse integer underflow.");
      }
      retval -= val;
    }
    return retval;
  } else {
    for (;c != me.end(); ++c) {
      int val = *c - '0';
      if (*c >= 'a' && *c <= 'z') {
        val = *c - 'a' + 10;
      }
      if (*c >= 'A' && *c <= 'Z') {
        val = *c - 'A' + 10;
      }
      if (val < 0 || val > base-1) {
        throw parse_error("Error parsing int.");
      }
      if (mxd10 < retval) {
        throw parse_error("Parse integer overflow.");
      }
      retval *= base;
      if (mx - val < retval) {
        throw parse_error("Parse integer overflow.");
      }
      retval += val;
    }
    return retval;
  }
}

template<class T>
constexpr int parse_int_16(T me) {
  return parse_int<T, 16>(me);
}

template <int n, class T>
constexpr typename impl::static_split<n, false, T>::return_type static_split(const T& m, char c) {
  return impl::static_split<n, false, T>::call(m, mes::make_mestring(&c, 1));
}

template <int n, class T>
constexpr typename impl::static_split<n, false, T>::return_type static_split(
    const T& m, const mes::mestring& c) {
  return impl::static_split<n, false, T>::call(m, c);
}

template <int n, class T>
constexpr typename impl::static_split<n, true, T>::return_type static_split_first(const T& m, char c) {
  return impl::static_split<n, true, T>::call(m, mes::make_mestring(&c, 1));
}

template <int n, class T>
constexpr typename impl::static_split<n, true, T>::return_type static_split_first(
    const T& m, const mes::mestring& c) {
  return impl::static_split<n, true, T>::call(m, c);
}

} // namespace mes
