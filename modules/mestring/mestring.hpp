#pragma once

#include <iostream>

namespace mes {

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

private:

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

constexpr mestring make_mestring(const char* c) {
  return mestring::make_mestring(c);
}

constexpr mestring make_mestring(const char* c, int n) {
  return mestring(c, n);
}

// For pretty-printing in general.
std::ostream &operator<<(std::ostream &out, const mestring &mes);

// For pretty-printing in mettle tests.
std::string to_printable(const mestring &mes);

} // namespace mes
