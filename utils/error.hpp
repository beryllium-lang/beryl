#pragma once
#include <string_view>
#include <cstdlib>

namespace beryl {
  inline void fail() { exit(1); }
  inline void term() { exit(0); }
  void throw_lex_error(std::string_view msg, int line_num, int col_num);
  void throw_lex_warning(std::string_view msg, int line_num, int col_num);
  void throw_arg_read_error(std::string_view msg);
  void throw_arg_read_warning(std::string_view args);
  void log(std::string_view msg);
} // namespace beryl