#include "mestring.hpp"

namespace mes {

std::ostream &operator<<(std::ostream &out, const mestring &mes) {
  out.write(mes.str_, mes.n_);
  return out;
}

std::string to_printable(const mestring &mes) {
  return "\"" + std::string(mes.str_, mes.n_) + "\"";
}

} // namespace mes
