#include "error.hpp"
#include <iostream>

namespace beryl {
  void throw_lex_error(std::string_view msg, int line_num, int col_num) {
    std::cerr << "error: " << msg << " at line " << line_num << " and column " << col_num << std::endl;
    fail();
  }

  void throw_lex_warning(std::string_view msg, int line_num, int col_num) {
    std::cerr << "warning: " << msg << " at line " << line_num << " and column " << col_num << std::endl;
  }

  void throw_arg_read_error(std::string_view msg) {
    std::cerr << "error: " << msg << '\n';
    fail();
  }

  void throw_arg_read_warning(std::string_view msg) {
    std::cerr << "warning: " << msg << '\n';
  }

  void log(std::string_view msg) {
    std::cerr << "log: " << msg << '\n';
  }
} // namespace beryl