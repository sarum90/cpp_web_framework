
#include "smhasher/MurmurHash3.h"
#include "mestring/mestring.hpp"
#include <iomanip>

namespace hash {

class mmh3;

inline mmh3 make_mmh3(mes::mestring str, uint32_t seed=0);
inline mmh3 combine_mmh3(const std::vector<mmh3>& vec);

class mmh3 {
public:

  bool operator==(const mmh3& other) const {
    return i_[0] == other.i_[0] && i_[1] == other.i_[1];
  }

  bool operator!=(const mmh3& other) const {
    return !((*this) == other);
  }

	bool operator<(const mmh3& other) const {
		if (i_[0] == other.i_[0]) {
      return i_[1] < other.i_[1];
		}
    return i_[0] < other.i_[0];
	}

  mmh3 combine_with(const mmh3& other) const {
    return combine_mmh3({(*this), other});
  }

  mmh3 unordered_combine_with(const mmh3& other) const {
    mmh3 out;
    if(__builtin_add_overflow(i_[0], other.i_[0], &out.i_[0])) {
      out.i_[0]++;
    }

    if(__builtin_add_overflow(i_[1], other.i_[1], &out.i_[1])) {
      out.i_[1]++;
    }

    return out;
  }

private:
  mmh3() {}

  uint64_t i_[2] = {0, 0};

  friend mmh3 make_mmh3(mes::mestring, uint32_t);
  friend mmh3 combine_mmh3(const std::vector<mmh3>& vec);

  template <class T>
  friend mmh3 unordered_combine_mmh3(const T& vec);

  friend std::ostream& operator<<(std::ostream& os, const mmh3& h);
};

inline mmh3 make_mmh3(mes::mestring str, uint32_t seed) {
  mmh3 ret;
  MurmurHash3_x64_128(str.begin(), str.size(), seed, &ret.i_[0]);
  return ret;
}

inline mmh3 combine_mmh3(const std::vector<mmh3>& vec) {
  mmh3 ret;
  MurmurHash3_x64_128(&vec[0], vec.size() * sizeof(mmh3), 0, &ret.i_[0]);
  return ret;
}

template <class T>
inline mmh3 unordered_combine_mmh3(const T& vec) {
  mmh3 ret;
  for (auto& h: vec) {
    ret = ret.unordered_combine_with(h);
  }

  return ret;
}

inline mmh3 unordered_combine_mmh3(const std::vector<mmh3>& vec) {
  return unordered_combine_mmh3<std::vector<mmh3>>(vec);
}

std::ostream& operator<<(std::ostream& os, const mmh3& h) {
  std::ios::fmtflags f(os.flags());
  os << std::hex << std::setfill('0') << std::setw(16) << h.i_[0];
  os << std::hex << std::setfill('0') << std::setw(16) << h.i_[1];
  os.flags(f);
  return os;
}

}
