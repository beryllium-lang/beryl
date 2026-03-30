#include "be1/biir-codegen/gen.hpp"
#include "be1/lexer/lex.hpp"
#include "be1/parser/parse.hpp"
#include "clparser.hpp"
#include "utils/arena.hpp"
#include "utils/error.hpp"
#include "utils/parse_json_params.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <sstream>

namespace fs = std::filesystem;

namespace beryl {
  void build(BuildArgs args) {
    beryl::Arena alloc(128 * 1024 * 2048);

    llvm::LLVMContext context;

    if (args.std_ver == Version{.major = 1, .minor = 0}) {
      for (const std::filesystem::path path : args.input_files) {
        llvm::Module mod("BerylliumModule", context);
        llvm::IRBuilder<> builder(context);

        std::string buf;
        if (!fs::exists(path)) {
          bool found = false;
          for (const auto& prefix : args.filepath_prefixes) {
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