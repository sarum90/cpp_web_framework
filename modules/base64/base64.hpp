#pragma once

#include "mestring.hpp"
#include <array>

namespace {

constexpr mes::mestring key{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_="};

constexpr std::array<int, 256> make_mapping() {
  int map[256] {};
  for (int i = 0; i < 256; i++) {
    map[i] = -1;
  }
  for(int i = 0; i < key.size(); i++) {
    if (key[i] != '=') {
      map[key[i]] = i;
    }
  }
  map['+'] = 62;
  map['/'] = 63;
  map['='] = -2;
  return std::array<int, 256>{
    map[0], map[1], map[2], map[3], map[4], map[5], map[6], map[7], map[8], map[9],
    map[10], map[11], map[12], map[13], map[14], map[15], map[16], map[17], map[18], map[19],
    map[20], map[21], map[22], map[23], map[24], map[25], map[26], map[27], map[28], map[29],
    map[30], map[31], map[32], map[33], map[34], map[35], map[36], map[37], map[38], map[39],
    map[40], map[41], map[42], map[43], map[44], map[45], map[46], map[47], map[48], map[49],
    map[50], map[51], map[52], map[53], map[54], map[55], map[56], map[57], map[58], map[59],
    map[60], map[61], map[62], map[63], map[64], map[65], map[66], map[67], map[68], map[69],
    map[70], map[71], map[72], map[73], map[74], map[75], map[76], map[77], map[78], map[79],
    map[80], map[81], map[82], map[83], map[84], map[85], map[86], map[87], map[88], map[89],
    map[90], map[91], map[92], map[93], map[94], map[95], map[96], map[97], map[98], map[99],
    map[100], map[101], map[102], map[103], map[104], map[105], map[106], map[107], map[108], map[109],
    map[110], map[111], map[112], map[113], map[114], map[115], map[116], map[117], map[118], map[119],
    map[120], map[121], map[122], map[123], map[124], map[125], map[126], map[127], map[128], map[129],
    map[130], map[131], map[132], map[133], map[134], map[135], map[136], map[137], map[138], map[139],
    map[140], map[141], map[142], map[143], map[144], map[145], map[146], map[147], map[148], map[149],
    map[150], map[151], map[152], map[153], map[154], map[155], map[156], map[157], map[158], map[159],
    map[160], map[161], map[162], map[163], map[164], map[165], map[166], map[167], map[168], map[169],
    map[170], map[171], map[172], map[173], map[174], map[175], map[176], map[177], map[178], map[179],
    map[180], map[181], map[182], map[183], map[184], map[185], map[186], map[187], map[188], map[189],
    map[190], map[191], map[192], map[193], map[194], map[195], map[196], map[197], map[198], map[199],
    map[200], map[201], map[202], map[203], map[204], map[205], map[206], map[207], map[208], map[209],
    map[210], map[211], map[212], map[213], map[214], map[215], map[216], map[217], map[218], map[219],
    map[220], map[221], map[222], map[223], map[224], map[225], map[226], map[227], map[228], map[229],
    map[230], map[231], map[232], map[233], map[234], map[235], map[236], map[237], map[238], map[239],
    map[240], map[241], map[242], map[243], map[244], map[245], map[246], map[247], map[248], map[249],
    map[250], map[251], map[252], map[253], map[254], map[255],
  };
}

constexpr auto rkey = make_mapping();

template <class T>
class base64_riterator {
public:
  constexpr base64_riterator(const T& it, const T& end): iterator_(it), end_iterator_(end) {
    set_get_char();
  }

  char operator*() const {
    return curr_char_;
  }

  void operator++() {
    set_get_char();
  }

  bool operator==(const base64_riterator& other) const {
    if (end_iterator_ != other.end_iterator_) {
      return false;
    }
    if (is_end_ != other.is_end_) {
      return false;
    }
    if (is_end_ && other.is_end_) {
      return true;
    }
    return iterator_ == other.iterator_;
  }

  bool operator!=(const base64_riterator& other) const {
    return !((*this) == other);
  }

private:

  void set_get_char() {
    curr_char_ = extra_;
    extra_ = 0;
    while (8 - bits_in_ > 6) {
      curr_char_ = curr_char_ << 6;
      int i = get_in();
      if (i == -2) {
        is_end_ = true;
        return;
      }
      curr_char_ += 0x3F & i;
      bits_in_ += 6;
      ++iterator_;
    }

    if (bits_in_ == 8) {
      bits_in_ = 0;
      return;
    }

    int i = get_in();
    ++iterator_;
    if (i == -2) {
      is_end_ = true;
      return;
    }

    char rem_bits = 8 - bits_in_;
    bits_in_ = 6 - rem_bits;

    curr_char_ = curr_char_ << rem_bits;
    curr_char_ += ((1 << rem_bits) - 1) & (i >> bits_in_);
    extra_ = ((1 << bits_in_) - 1) & i;
  }

  int get_in() {
    while(iterator_ != end_iterator_ && rkey[*iterator_] == -1) {
      ++iterator_;
    }
    if (iterator_ == end_iterator_) {
      return -2;
    }
    return rkey[*iterator_];
  }


  T iterator_;
  T end_iterator_;
  char curr_char_ = 0;
  char extra_ = 0;
  bool is_end_ = false;
  char bits_in_ = 0;
};



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

class base64_dencoded {
public:
  typedef base64_riterator<mes::mestring_cat::iterator> iterator;

  base64_dencoded(mes::mestring_cat str): str_(str) {}

  iterator begin() const {
    if (str_.size() == 0) {
      return end();
    }
    return iterator(str_.begin(), str_.end());
  }

  iterator end() const {
    return iterator(str_.end(), str_.end());
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

private:
  mes::mestring_cat str_;
};

// For pretty-printing in mettle tests.
inline std::string to_printable(const base64_encoded &be) {
  return "\"" + std::string(be) + "\"";
}

inline std::ostream &operator<<(std::ostream &out, const base64_encoded &be) {
  out << std::string(be);
  return out;
}

inline std::string to_printable(const base64_dencoded &be) {
  return "\"" + std::string(be) + "\"";
}

inline std::ostream &operator<<(std::ostream &out, const base64_dencoded &be) {
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

inline decltype(auto) base64_decode(mes::mestring_cat input) {
  return base64_dencoded{input};
}
