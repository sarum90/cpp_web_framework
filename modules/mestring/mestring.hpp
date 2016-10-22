#pragma once



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

  static constexpr mestring make_mestring(const char * c) {
    int size = -1;
    if (c) {
      while (c[++size]);
    }
    return mestring(c, size);
  }

private:

  constexpr mestring(const char * c, int n): str_(c), n_(n) { }


  const char * str_;
  const int n_;

};

constexpr mestring make_mestring(const char* c) {
  return mestring::make_mestring(c);
}
