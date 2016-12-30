#pragma once

#include "mestring.hpp"

namespace {

constexpr mes::mestring key{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_="};

template <class T>
class base64_iterator {
public:

  constexpr base64_iterator(const T& it, const T& end, bool eqs):
    iterator_(it), end_iterator_(end), eqs_(eqs) {}

  char operator*() const {
    return key[get_bits(6)];
  }

  void operator++() {
    if (bits_remaining_ > 6) {
      bits_remaining_ -= 6;
    } else {
      int num = 6 - bits_remaining_;
      if (iterator_ != end_iterator_) {
        ++iterator_;
      }
      bits_remaining_ = 8 - num;
    }
  }

  bool operator==(const base64_iterator& other) const {
    if (end_iterator_ != other.end_iterator_ || eqs_ != other.eqs_) {
      return false;
    }
    if (!eqs_ && iterator_ == end_iterator_) {
      return other.iterator_ == end_iterator_;
    }
    return (iterator_ == other.iterator_) && (bits_remaining_ == other.bits_remaining_);
  }

  bool operator!=(const base64_iterator& other) const {
    return !((*this) == other);
  }

private:
  unsigned char get_bits(int num) const {
    if(iterator_ == end_iterator_) {
      return 64;
    }
    unsigned char ret = 0;
    auto br = bits_remaining_;
    auto it = iterator_;
    if (br <= num) {
      unsigned char mask = (1 << br) - 1;
      ret = (*it) & mask;
      num -= br;
      br = 0;
      ++it;
      if (num == 0) {
        return ret;
      }

      if (it == end_iterator_) {
        return ret << num;
      }
      br = 8;
    }
    if (num <= br) {
      ret = (ret << num);
      unsigned char c = *it;
      c = c >> (br - num);
      unsigned char mask = (1 << num) - 1;
      ret += c & mask;
      br -= num;
    }
    return ret;
  }

  T iterator_;
  T end_iterator_;
  bool eqs_;
  int bits_remaining_ = 8;
};

template<class T>
base64_iterator<T> make_base64_iterator(const T& t, const T& end, bool eqs) {
  return base64_iterator<T>(t, end, eqs);
}

class base64_encoded {
public:
  typedef base64_iterator<mes::mestring_cat::iterator> iterator;

  base64_encoded(mes::mestring_cat str, bool eqs): str_(str), eqs_(eqs) {}

  iterator begin() const {
    if (str_.size() == 0) {
      return end();
    }
    return make_base64_iterator(str_.begin(), str_.end(), eqs_);
  }

  iterator end() const {
    return make_base64_iterator(str_.end(), str_.end(), eqs_);
  }

  operator std::string() const {
    std::string str;
    for (char c : (*this)) {
      str += c;
    }
    return str;
  }

  template<class T>
  bool operator==(const T& t) const {
    auto it = begin();
    auto oit = t.begin();
    while(it != end() && oit != t.end()) {
      if (*it != *oit) {
        return false;
      }
      ++it;
      ++oit;
    }
    return it == end() && oit == t.end();
  }

  template <int N>
  bool operator==(char const (&c)[N]) const {
    return (*this) == mes::mestring(c);
  }

  int size() const {
    if (str_.size() == 0) {
      return 0;
    }
    auto n = 1 + (str_.size() * 8 - 1) / 6;
    if (!eqs_) {
      return n;
    }
    auto r = (str_.size() * 8) % 6;
    if (r == 0) {
      r = 6;
    }
    return n + (6 - r) / 2;
  }
  

private:
  mes::mestring_cat str_;
  bool eqs_;
};

// For pretty-printing in mettle tests.
inline std::string to_printable(const base64_encoded &be) {
  return "\"" + std::string(be) + "\"";
}

inline std::ostream &operator<<(std::ostream &out, const base64_encoded &be) {
  out << std::string(be);
  return out;
}

}

inline decltype(auto) base64_encode(mes::mestring_cat input) {
  return base64_encoded{input, true};
}


inline decltype(auto) base64_encode_noeq(mes::mestring_cat input) {
  return base64_encoded{input, false};
}
