#pragma once

#include <iostream>
#include <vector>

namespace mes {

struct mestring;

constexpr mestring make_mestring(const char* c, int n);

struct mestring {

  constexpr bool operator==(const char * c) const{
    if (!c)
      return c == str_;
    for(int i = 0; i < n_; i++) {
      if (c[i] != str_[i] || !c[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr int find_index(char c) const {
    int i = 0;
    for (auto ch : (*this)) {
      if (ch == c) {
        return i;
      }
      i++;
    }
    return -1;
  }

  constexpr bool operator!=(const char * c) const{
    return !((*this) == c);
  }

  constexpr bool operator==(const std::string& s) const{
    if (s.size() != n_) {
      return false;
    }
    for(int i = 0; i < n_; i++) {
      if (s[i] != str_[i]) {
        return false;
      }
    }
    return true;
  }

  operator std::string() const {
    return std::string{str_, static_cast<unsigned long>(n_)};
  }

  constexpr bool operator!=(const std::string& s) const{
    return !((*this) == s);
  }

  constexpr bool operator==(const mestring& m) const{
    if (m.n_ != n_) {
      return false;
    }
    for(int i = 0; i < n_; i++) {
      if (m.str_[i] != str_[i]) {
        return false;
      }
    }
    return true;
  }

  constexpr bool operator!=(const mestring& m) const{
    return !((*this) == m);
  }

  constexpr const char& operator[](int index) const {
    if (index > n_) {
      throw std::runtime_error("Access off end of mestring.");
    } 
    if (index < 0) {
      throw std::runtime_error("Attempted negative mestring access.");
    }
    return str_[index];
  }

  constexpr int size() const {
    return n_;
  }

  constexpr const char * begin() const {
    return str_;
  }

  constexpr const char * end() const {
    return begin() + n_;
  }

  // Assuming that this is a string literal, or at least that it has longer
  // lifetime than this object.
  template <int N>
  constexpr mestring(const char (&c)[N]): str_(&c[0]), n_(getlen(&c[0], N)) { }

private:

  static constexpr int getlen(const char * c, int len) {
    if (c[len-1] != '\0') {
      throw std::runtime_error("Can only construct mestrings from string literals.");
    }
    return len-1;
  }

  constexpr mestring(const char * c, int n): str_(c), n_(n) { }

  static constexpr mestring make_mestring(const char * c) {
    int size = -1;
    if (c) {
      while (c[++size]);
    }
    return mestring(c, size);
  }

  const char * str_;
  const int n_;

  friend std::ostream &operator<<(std::ostream &out, const mestring &mes);
  friend std::string to_printable(const mestring &mes);
  friend constexpr mestring make_mestring(const char* c);
  friend constexpr mestring make_mestring(const char* c, int n);

};

class mestring_cat {
public:

  mestring_cat(){}
  mestring_cat(mestring ms)
  {
    (*this) += ms;
  }

  mestring_cat(const mestring_cat&) = default;
  mestring_cat& operator=(const mestring_cat&) = default;

  mestring_cat(mestring_cat&&) = default;
  mestring_cat& operator=(mestring_cat&&) = default;

  class iterator {
  public:
    iterator(
      const std::vector<mestring>::const_iterator& end,
      const std::vector<mestring>::const_iterator& it,
      const char * iit
    ): end_(end), it_(it), iit_(iit){}

    iterator(iterator&&) = default;
    iterator& operator=(iterator&&) = default;

    iterator(const iterator&) = default;
    iterator& operator=(const iterator&) = default;

    bool operator==(const iterator& other) const {
      if (iit_ == nullptr) {
        return other.iit_ == nullptr;
      }
      return it_ == other.it_ && iit_ == other.iit_;
    }

    bool operator!=(const iterator& other) const {
      return !((*this) == other);
    }

    const char& operator*() {
      return *iit_;
    }

    iterator& operator++() {
      if (iit_ == nullptr) {
        return (*this);
      }
      iit_++;
      while(true) {
        if (iit_ != it_->end()) {
          return (*this);
        }
        ++it_;
        if (it_ == end_) {
          iit_ = nullptr;
          return (*this);
        }
        iit_ = it_->begin();
      }
    }

  private:
    const std::vector<mestring>::const_iterator end_;
    std::vector<mestring>::const_iterator it_;
    const char * iit_;

    friend mestring_cat substr(mestring_cat::iterator s, mestring_cat::iterator e);
  };

  iterator begin() const {
    std::vector<mestring>::const_iterator e = strs_.end();
    std::vector<mestring>::const_iterator b = strs_.begin();
    return iterator{e, b, b->begin()};
  }

  iterator end() const {
    std::vector<mestring>::const_iterator e = strs_.end();
    return iterator{e, e, nullptr};
  }

  operator std::string() const {
    std::string ret;
    for (auto& s: strs_) {
      ret += std::string(s);
    }
    return ret;
  }

  mestring_cat& operator+=(const mestring& str) {
    if (str.size() > 0) {
      strs_.push_back(str);
    }
    return (*this);
  }

  bool operator==(const mestring& m) {
    if (size() != m.size()) {
      return false;
    }
    auto i = begin();
    auto ii = m.begin();
    while (i != end()) {
      if (*i != *ii) {
        return false;
      }
      ++i;
      ++ii;
    }
    return true;
  }

  const char& operator[](int n) {
    auto i = strs_.begin();
    while (i != strs_.end()) {
      if (n < i->size()) {
        return (*i)[n];
      }
      n -= i->size();
      ++i;
    }
    throw std::runtime_error("Access off end of mestring_cat.");
  }

  int size() const {
    int sz = 0;
    for (auto& s: strs_) {
      sz += s.size();
    }
    return sz;
  }

  static mestring_cat truncated(mestring_cat&& other, int size) {
    if (size < 0) {
      size = other.size() + size;
    }
    if (size < 0 || size  > other.size()) {
      throw std::runtime_error("Invalid truncate size");
    }
    mestring_cat ret;
    int new_size = 0;
    auto i = other.strs_.begin();
    while(i != other.strs_.end() && i->size() + new_size < size) {
      ret += *i;
      new_size += i->size();
      ++i;
    }
    const char * ms = &(*i)[0];
    ret += mes::make_mestring(ms, size - new_size);
    return ret;
  }

  void reset() {
    strs_.clear();
  }

  const std::vector<mestring>& mestrings() const {
    return strs_;
  }

private:
  std::vector<mestring> strs_;

  friend std::ostream &operator<<(std::ostream &out, const mestring_cat &mes);
};

constexpr mestring make_mestring(const char* c) {
  return mestring::make_mestring(c);
}

constexpr mestring make_mestring(const char* c, int n) {
  return mestring(c, n);
}

// For pretty-printing in general.
std::ostream &operator<<(std::ostream &out, const mestring &mes);

std::ostream &operator<<(std::ostream &out, const mestring_cat &mes);

// For pretty-printing in mettle tests.
std::string to_printable(const mestring &mes);

template <class T>
constexpr inline decltype(auto) find_it(const T& str, mestring m) {
  auto s = str.begin();
  if (m.size() == 0) {
    return s;
  }
  char m1 = m[0];
  while(s != str.end()) {
    if (*s == m1) {
      auto mi = m.begin();
      auto ss = s;
      while (mi != m.end() && ss != str.end() && *mi == *ss) {
        ++mi;
        ++ss;
        if (mi == m.end()) {
          return s;
        }
      }
    }
    ++s;
  }
  return s;
}

template <class T>
constexpr inline int find_index(const T& str, mestring m) {
  auto s = str.begin();
  if (m.size() == 0) {
    return 0;
  }
  char m1 = m[0];
  int index = 0;
  while(s != str.end()) {
    if (*s == m1) {
      auto mi = m.begin();
      auto ss = s;
      while (mi != m.end() && ss != str.end() && *mi == *ss) {
        ++mi;
        ++ss;
        if (mi == m.end()) {
          return index;
        }
      }
    }
    ++s;
    ++index;
  }
  return -1;
}


} // namespace mes
