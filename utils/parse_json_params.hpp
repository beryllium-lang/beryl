#pragma once
#include <string>
#include <vector>

namespace beryl {
  std::vector<std::string> get_includes(std::string_view path);
}