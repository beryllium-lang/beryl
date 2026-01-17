#include "error.hpp"
#include <iostream>

namespace beryl {
void throw_compiler_error(std::string_view msg) {
  std::cerr << "Error: " << msg;
  fail();
}
} // namespace beryl