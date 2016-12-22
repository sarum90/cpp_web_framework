#pragma once

namespace mes {

struct parse_error: public std::runtime_error{
  using std::runtime_error::runtime_error;
};

inline constexpr mestring substr(const char * s, const char * e) {
  return make_mestring(s, e-s);
}

inline mestring_cat substr(mestring_cat::iterator s, mestring_cat::iterator e) {
  mestring_cat mc;

  if (s.it_ == e.it_) {
    mc += substr(s.iit_, e.iit_);
    return mc;
  }
  auto i = s.it_;
  mc += substr(s.iit_, i->end());
  ++i;

  while(i != e.it_ && i != e.end_) {
    mc += *i;
    ++i;
  }

  if (i != s.end_) {
    mc += substr(e.it_->begin(), e.iit_);
  }

  return mc;
}




// TODO(marcus): This could probably be cleaner...
namespace impl {

  template <int n, bool b, class T>
  struct static_split {
    using return_type = decltype(
        std::tuple_cat(
          std::declval<typename static_split<n-1, b, T>::return_type>(),
          std::declval<std::tuple<mestring>>()
        )
    );

    constexpr static return_type call(const T& m, mes::mestring c) {
      int ind = 0;
      auto i = find_it(m, c);
      if (i == m.end()) {
        throw parse_error("Not enough separators for static_split.");
      }

      auto first = substr(m.begin(), i);
      for(int iii = 0; iii < c.size(); iii++) {
        ++i;
      }

      return std::tuple_cat(
          std::make_tuple(first),
          static_split<n-1, b, T>::call(substr(i, m.end()), c)
      );
    }
  };

  template <bool b, class T>
  struct static_split<1, b, T> {
    using return_type = std::tuple<T>;

    constexpr static return_type call(const T& m, mes::mestring c) {
      auto i = find_it(m, c);
      if (i != m.end() && !b) {
        throw parse_error("Too many separators for static_split.");
      }
      return std::make_tuple(m);
    }
  };

  template <bool b, class T>
  struct static_split<2, b, T> {
    using return_type = std::tuple<T, T>;

    constexpr static return_type call(const T& m, const mes::mestring& c) {
      auto i = find_it(m, c);
      if (i == m.end()) {
        throw parse_error("Not enough separators for static_split.");
      }
      auto first = substr(m.begin(), i);
      for(int iii = 0; iii < c.size(); iii++) {
        ++i;
      }
      auto second = substr(i, m.end());
      auto si = find_it(second, c);
      if (si != m.end() && !b) {
        throw parse_error("Too many separators for static_split.");
      }

      return std::make_tuple(first, second);
    }
  };

} // namespace impl
} // namespace mes

