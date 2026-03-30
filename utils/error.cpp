#include "error.hpp"
#include <iostream>

namespace beryl {
  void throw_lex_error(std::string_view msg, int line_num, int col_num) {
    std::cerr << "\e[31merror: \e[0m" << msg << " at line " << line_num << " and column " << col_num << std::endl;
    fail();
  }

  void throw_lex_warning(std::string_view msg, int line_num, int col_num) {
    std::cerr << "\e[35mwarning: \e[0m" << msg << " at line " << line_num << " and column " << col_num << std::endl;
  }

  void throw_arg_read_error(std::string_view msg) {
    std::cerr << "\e[31merror: \e[0m" << msg << '\n';
    fail();
  }

  void throw_arg_read_warning(std::string_view msg) {
    std::cerr << "\e[35mwarning: \e[0m" << msg << '\n';
  }

  void log(std::string_view msg) {
    std::cerr << "\e[32mlog: \e[0m" << msg << '\n';
  }
} // namespace beryl