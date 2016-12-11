#pragma once

namespace mes {

struct parse_error: public std::runtime_error{
  using std::runtime_error::runtime_error;
};

// TODO(marcus): This could probably be cleaner...
namespace impl {
  template <int n>
  struct static_split {
    using return_type = decltype(
        std::tuple_cat(
          std::declval<typename static_split<n-1>::return_type>(),
          std::declval<std::tuple<mestring>>()
        )
    );

    constexpr static return_type call(const mestring& m, char c) {
      int ind = 0;
      auto i = m.begin();
      for(; i != m.end() && *i != c; ind++, i++) { }
      if (i == m.end()) {
        throw parse_error("Not enough separators for static_split.");
      }

      return std::tuple_cat(
          std::make_tuple(make_mestring(m.begin(), ind)),
          static_split<n-1>::call(make_mestring(&m[ind+1], m.size()-ind-1), c)
      );
    }
  };

  template <>
  struct static_split<1> {
    using return_type = std::tuple<mestring>;

    constexpr static return_type call(const mestring& m, char c) {
      int n = 0;
      auto i = m.begin();
      for(; i != m.end() && *i != c; n++, i++);
      if (i != m.end()) {
        throw parse_error("Too many separators for static_split.");
      }
      return std::make_tuple(m);
    }
  };

  template <>
  struct static_split<2> {
    using return_type = std::tuple<mestring, mestring>;

    constexpr static return_type call(const mestring& m, char c) {
      int n = 0;
      auto i = m.begin();
      for(; i != m.end() && *i != c; n++, i++);
      if (i == m.end()) {
        throw parse_error("Not enough separators for static_split.");
      }
      ++i;
      for(; i != m.end() && *i != c; i++);
      if (i != m.end()) {
        throw parse_error("Too many separators for static_split.");
      }

      return std::make_tuple(make_mestring(m.begin(), n), make_mestring(&m[n+1], m.size()-n-1));
    }
  };

} // namespace impl
} // namespace mes

