#include "clparser.hpp"
#include "utils/error.hpp"
#include "utils/parse_json_params.hpp"
#include <filesystem>
#include <iostream>
#include <optional>
#include <string_view>

namespace fs = std::filesystem;

namespace beryl {
  static bool directory_exists(const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
  }

  void run(int argc, char* argv[]) {
    if (argc < 2) {
      beryl::help();
      return;
    }
    std::string_view command = argv[1];
    CompileArgs args{argc, argv};
    if (command == "create") {
      bool install_stdlib = true;
      for (size_t i = 2; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--no-stdlib") install_stdlib = true;
        else throw_arg_read_warning("unknown flag");
      }
      create_venv(install_stdlib);
    } else if (command == "destroy") {
      bool force = false;
      for (size_t i = 2; i < argc; ++i) {
        std::string_view arg = argv[i];
        if (arg == "--force") force = true;
        else throw_arg_read_warning("unknown flag");
      }
      destroy_venv(force);
    } else if (command == "build") {
      if (!directory_exists("__bervenv__")) {
        std::cerr << "Bervenv does not exist. Would you like to create it? Y/n: ";
        char c;
        std::cin >> c;
        if (c == 'Y' || c == 'y') {
          create_venv();
        } else beryl::fail();
      }
      std::vector<std::string> paths_to_by_file;
      std::vector<std::string> includes;
      std::optional<fs::path> exec{};
      Version ver{.major = 1, .minor = 0};
      bool link = true;
      bool force_module_recompile = false;
      OptLevel opt_level = OptLevel::NONE;

      for (size_t i = 2; i < args.argc; ++i) {
        std::string arg = args.argv[i];
        if (arg == "--no-link") link = false;
        else if (arg == "--force-module-recompile") force_module_recompile = true;
        else if (arg == "-O0") opt_level = OptLevel::NONE;
        else if (arg == "-O1") opt_level = OptLevel::ONE;
        else if (arg == "-O2") opt_level = OptLevel::TWO;
        else if (arg == "-O3") opt_level = OptLevel::THREE;
        else if (arg.rfind("-includes=", 0) == 0) includes = beryl::get_includes(arg.substr(10));
        else if (arg == "-o") {
          if (i == args.argc - 1) throw_arg_read_error("-o needs a file name to specify output");
          ++i;
          exec = args.argv[i];
        } else if (arg.rfind("-std=", 0) == 0) {
          std::string verstr = arg.substr(5);
          if (verstr == "be1") ver = {.major = 1, .minor = 0};
          else beryl::throw_arg_read_error("unknown version: " + verstr);
        } else if (fs::path(arg).extension() == ".by") paths_to_by_file.emplace_back(arg);
        else throw_arg_read_warning("unknown argument: " + arg);
      }

      if (paths_to_by_file.size() == 0) beryl::throw_arg_read_error("There is no .by file to compile");
      if (paths_to_by_file.size() > 1 && exec.has_value()) beryl::throw_arg_read_error("Cannot redirect output for multiple files");
      build(BuildArgs{link, force_module_recompile, opt_level, includes, paths_to_by_file, exec.has_value() ? exec.value() : "", ver});
    } else if (command == "help" || command == "--help" || command == "-h") {
      help();
    } else if (command == "version" || command == "--version" || command == "-v") {
      version();
    } else {
      beryl::throw_arg_read_error("You typed an unknown command");
    }
  }
} // namespace beryl