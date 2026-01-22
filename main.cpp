#include "utils/error.hpp"
#include "utils/parse_json_params.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <filesystem>
#include <iostream>
#include <vector>
#include <optional>

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  std::cout << std::boolalpha;
  auto directory_exists = [](const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
  };
  auto is_mode = [argv](std::string_view str) {
    return str == argv[1];
  };
  if (argc == 1)
    beryl::throw_arg_read_error(
      "Can choose beryl build, beryl create, or beryl remove");
  if (is_mode("create")) {
    if (directory_exists("__bervenv__"))
      beryl::throw_arg_read_error("__bervenv__ folder already exists. Please "
        "choose a different name.");
    fs::create_directory("__bervenv__");
    fs::create_directory("__bervenv__/syspacks");
    fs::create_directory("__bervenv__/build");
  } else if (is_mode("destroy")) {
    if (!directory_exists("__bervenv__"))
      beryl::throw_arg_read_error("Bervenv does not exist to remove");
    fs::remove_all("__bervenv__");
  } else if (is_mode("build")) {
    if (!directory_exists("__bervenv__")) beryl::throw_arg_read_error("No bervenv to compile in");
    std::vector<fs::path> paths_to_by_file;
    std::vector<std::string> includes;
    std::optional<fs::path> exec{};
    int ver = 1;
    bool link = true;
    bool force_module_recompile = false;

    for (size_t i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (fs::path(arg).extension() == ".by") paths_to_by_file.emplace_back(arg);
      else if (arg == "--no-link") link = false;
      else if (arg == "--force-module-recompile") force_module_recompile = true;
      else if (arg.rfind("-includes=", 0) == 0) includes = beryl::get_includes(arg.substr(10));
      else if (arg.rfind("-out=", 0) == 0)
        exec = arg.substr(6);
      else if (arg.rfind("-std=be", 0) == 0) {
        std::string verstr = arg.substr(8);
        ver = std::stoi(verstr);
      } else beryl::throw_arg_read_warning("Unknown compiler argument: " + arg);
    }

    if (paths_to_by_file.size() == 0) beryl::throw_arg_read_error("There is no .by file to compile");
    if (paths_to_by_file.size() > 1 && exec.has_value())
      beryl::throw_arg_read_error("Cannot redirect output for multiple files");

    llvm::LLVMContext context;

    for (const std::filesystem::path& path : paths_to_by_file) {
      llvm::Module mod("BerylliumModule", context);
      llvm::IRBuilder<> builder(context);
    }
  } else beryl::throw_arg_read_error("Invalid option");
  return 0;
}
