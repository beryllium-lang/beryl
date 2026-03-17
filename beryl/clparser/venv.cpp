#include "clparser.hpp"
#include "utils/error.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace beryl {
  static bool directory_exists(const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
  }

  void create_venv(bool install_stdlib) {
    if (directory_exists("__bervenv__"))
      beryl::throw_arg_read_error(
          "__bervenv__ folder already exists. Please "
          "delete the existing venv or use a different directory.");
    fs::create_directory("__bervenv__");
    fs::create_directory("__bervenv__/syspacks");
    fs::create_directory("__bervenv__/build");
    if (install_stdlib) {
      std::cout << "Installing standard library...\n";
      std::system("mineraloil install std");
    }
  }

  void destroy_venv(bool force) {
    if (!directory_exists("__bervenv__")) beryl::throw_arg_read_error("Bervenv does not exist to remove");
    std::error_code ec;
    bool do_rem = force || ask();
    if (do_rem) {
      std::uintmax_t count = fs::remove_all("__bervenv__", ec);
      if (count == static_cast<std::uintmax_t>(-1) || ec.value() != 0) beryl::throw_arg_read_error("Failed to destroy venv: " + ec.message());
    }
  }
} // namespace beryl