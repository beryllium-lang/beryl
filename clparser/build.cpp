#include "be1/biir-codegen/gen.hpp"
#include "be1/lexer/lex.hpp"
#include "be1/parser/parse.hpp"
#include "clparser.hpp"
#include "utils/arena.hpp"
#include "utils/error.hpp"
#include "utils/parse_json_params.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace fs = std::filesystem;

namespace beryl {
  static bool directory_exists(const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
  }

  void build(CompileArgs args) {
    beryl::Arena alloc(128 * 1024 * 1024);
    if (!directory_exists("__bervenv__")) {
      std::cerr << "Bervenv does not exist. Would you like to create it? Y/n: ";
      char c;
      std::cin >> c;
      if (c == 'Y' || c == 'y') {
        create_venv(args);
      } else
        beryl::fail();
    }
    std::vector<fs::path> paths_to_by_file;
    std::vector<std::string> includes;
    std::optional<fs::path> exec{};
    Version ver{.major = 1, .minor = 0};
    bool link = true;
    bool force_module_recompile = false;
    OptLevel opt_level = OptLevel::NONE;

    for (size_t i = 2; i < args.argc; ++i) {
      std::string arg = args.argv[i];
      if (fs::path(arg).extension() == ".by")
        paths_to_by_file.emplace_back(arg);
      else if (arg == "--no-link")
        link = false;
      else if (arg == "--force-module-recompile")
        force_module_recompile = true;
      else if (arg == "-O0")
        opt_level = OptLevel::NONE;
      else if (arg == "-O1")
        opt_level = OptLevel::ONE;
      else if (arg == "-O2")
        opt_level = OptLevel::TWO;
      else if (arg == "-O3")
        opt_level = OptLevel::THREE;
      else if (arg.rfind("-includes=", 0) == 0)
        includes = beryl::get_includes(arg.substr(10));
      else if (arg.rfind("-out=", 0) == 0)
        exec = arg.substr(5);
      else if (arg.rfind("-std=", 0) == 0) {
        std::string verstr = arg.substr(5);
        if (verstr == "be1")
          ver = {.major = 1, .minor = 0};
        else
          beryl::throw_arg_read_error("Unknown version: " + verstr);
      } else
        beryl::throw_arg_read_warning("Unknown compiler argument: " + arg);
    }

    if (paths_to_by_file.size() == 0)
      beryl::throw_arg_read_error("There is no .by file to compile");
    if (paths_to_by_file.size() > 1 && exec.has_value())
      beryl::throw_arg_read_error("Cannot redirect output for multiple files");

    llvm::LLVMContext context;

    if (ver == Version{.major = 1, .minor = 0}) {
      for (const std::filesystem::path& path : paths_to_by_file) {
        llvm::Module mod("BerylliumModule", context);
        llvm::IRBuilder<> builder(context);

        std::string buf;
        if (!fs::exists(path)) {
          bool found = false;
          for (const auto& prefix : includes) {
            if (auto appended_path = prefix / path; fs::exists(appended_path)) {
              std::ifstream file(appended_path);
              if (file.is_open() && file.good()) {
                std::ostringstream tmp;
                tmp << file.rdbuf();
                buf = tmp.str();
                found = true;
                break;
              }
            }
          }
          if (!found) beryl::throw_arg_read_error("Could not find file: " + path.string());
        } else {
          std::ifstream file(path);
          if (file.is_open() && file.good()) {
            std::ostringstream tmp;
            tmp << file.rdbuf();
            buf = tmp.str();
          } else {
            beryl::throw_arg_read_error("Failed to open file: " + path.string());
          }
        }

        be1::TokenStream tokens = beryl::be1::lex(buf, path.string());
        be1::ast::Program* program = beryl::be1::parse(tokens, alloc);
        std::string biir = be1::ast_to_biir(program);
      }
    }
  }
} // namespace beryl