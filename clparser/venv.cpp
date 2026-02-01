#include "clparser.hpp"
#include "utils/error.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

namespace beryl {
  static bool directory_exists(const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
  }

  void create_venv(CompileArgs args) {
    bool install_stdlib = true;
    if (args.argc > 2 && std::string(args.argv[2]) == "--no-stdlib") {
      install_stdlib = false;
    }
    if (directory_exists("__bervenv__"))
      beryl::throw_arg_read_error(
        "__bervenv__ folder already exists. Please "
        "delete the existing venv or use a different directory.");
    try {
      fs::create_directory("__bervenv__");
      fs::create_directory("__bervenv__/syspacks");
      fs::create_directory("__bervenv__/build");
    } catch (const std::filesystem::filesystem_error& e) {
      fs::remove_all("__bervenv__");
      beryl::throw_arg_read_error("Failed to create venv: " + std::string(e.what()));
      std::cerr << "Bervenv creation failed. Cleanup done.\n";
    }
    if (install_stdlib) {
      std::cout << "Installing standard library...\n";
      try {
        std::system("mineraloil install stdlib");
      } catch (const std::exception& e) {
        beryl::throw_arg_read_error("Failed to install standard library: " + std::string(e.what()));
      }
    }
  }

  void destroy_venv(CompileArgs args) {
    if (!directory_exists("__bervenv__"))
      beryl::throw_arg_read_error("Bervenv does not exist to remove");
    std::error_code ec;
    bool do_rem = false;
    if (args.argc > 2) {
      if (std::string(args.argv[2]) == "--force") do_rem = true;
    } else
      do_rem = ask();
    if (do_rem) {
      std::uintmax_t count = fs::remove_all("__bervenv__", ec);
      if (count == static_cast<std::uintmax_t>(-1) || ec.value() != 0)
        beryl::throw_arg_read_error("Failed to destroy venv: " + ec.message());
    }
  }
}