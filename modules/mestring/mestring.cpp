#include "mestring.hpp"

namespace mes {

std::ostream &operator<<(std::ostream &out, const mestring &mes) {
  out.write(mes.str_, mes.n_);
  return out;
}

std::ostream &operator<<(std::ostream &out, const mestring_cat &mes) {
  for (auto& ms: mes.strs_) {
    out << ms;
  }
  return out;
}

std::string to_printable(const mestring &mes) {
  return "\"" + std::string(mes.str_, mes.n_) + "\"";
}

} // namespace mes
