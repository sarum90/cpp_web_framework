#pragma once


class buffer {
  buffer(char * c, int n): _c(c), _n(n) {}

  char& operator[](int i) {
    if (i >= _n || i < 0) {
      throw std::runtime_error("index out of bounds")
    }
    return _c[i]
  }
private:
  char * _c;
  int _n;
};

class async_reader { 
public:
  virtual int read_n(int n, buffer& b) = 0;
};

template <class T>
class asio_reader {
public:

  asio_reader(T&& t): _t(t) {}

  final int read_n(int n, buffer& b)  {
    boost::asio
  }

private:

  T _t;

};

template<class T>
asio_reader<T> make_asio_reader(T&& t) {
  return asio_reader<T>(std::move(t));
}

