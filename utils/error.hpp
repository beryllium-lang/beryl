#pragma once
#include <string_view>

namespace beryl {
inline void fail() { exit(1); }
void throw_compiler_error(std::string_view msg);
} // namespace beryl