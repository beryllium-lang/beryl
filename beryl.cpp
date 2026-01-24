#include "utils/error.hpp"
#include "utils/parse_json_params.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>

struct Version {
  unsigned int major;
  unsigned int minor;

  bool operator<(Version other) const {
    if (major < other.major) return true;
    else if (major > other.major) return false;
    else if (minor < other.minor) return true;
    else return false;
  }

  bool operator>(Version other) const {
    if (major > other.major) return true;
    else if (major < other.major) return false;
    else if (minor > other.minor) return true;
    else return false;
  }

  bool operator==(Version other) const { return major == other.major && minor == other.minor; }
  bool operator!=(Version other) const { return !operator==(other); }
  bool operator<=(Version other) const { return !operator>(other); }
  bool operator>=(Version other) const { return !operator<(other); }
};

enum class OptLevel { NONE, ONE, TWO, THREE };
struct Token {
  enum {
    SEMI,
    INT_LIT,
    FLOAT_LIT,
    VAR,
    IDENT,
    IF,
    WHILE,
    DO,
    COLON,
    COMMA,
    OPEN_ANGLE,
    CLOSE_ANGLE,
    FUNC,
    OPEN_PAREN,
    CLOSE_PAREN,
    EXIT,
    RETURN,
    PLUS,
    MINUS,
    ASTER,
    FORW_SLASH,
    BACK_SLASH,
    MOD,
    MUT,
    LEFT_SHIFT,
    RIGHT_SHIFT,
    LESS,
    GREATER,
    OPEN_CURLY,
    CLOSE_CURLY,
    FOR,
    REPEAT,
    BAND,
    BOR,
    BXOR,
    BXNOT,
    AS,
    UNSAFE,
    AND,
    OR,
    NOT,
    EQ,
    IMPORT,
    BOOL,
    CHAR,
    BYTE,
    INT,
    LONG,
    QINT,
    UBYTE,
    UINT,
    ULONG,
    UQINT,
    FLOAT,
    DOUBLE,
    NMSP, // namespace
    STRING,
    BYTESTREAM,
    ARR,
    DYNARR,
    ADR,
    PTR,
    RCPTR,
    WEAKPTR,
    UNSAFEPTR,
    REF,
    RESULT,
    TUP,
    HEXC,
    UNIC,
    TYPEOF,

  } type;

  std::string metadata;
};

namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
  std::cout << std::boolalpha;
  auto directory_exists = [](const fs::path& path) {
    return fs::exists(path) && fs::is_directory(path);
  };
  auto is_mode = [argv](std::string_view str) { return str == argv[1]; };
  auto ask = []() {
    std::cout << "Are you sure? Y/n: ";
    char c;
    std::cin >> c;
    return c == 'Y' || c == 'y';
  };
  if (argc == 1)
    beryl::throw_arg_read_error(
      "Can choose beryl build, beryl create, or beryl destroy");
  if (is_mode("create")) {
    if (directory_exists("__bervenv__"))
      beryl::throw_arg_read_error("__bervenv__ folder already exists. Please "
        "delete the existing venv or use a different directory.");
    fs::create_directory("__bervenv__");
    fs::create_directory("__bervenv__/syspacks");
    fs::create_directory("__bervenv__/build");
  } else if (is_mode("destroy")) {
    if (!directory_exists("__bervenv__"))
      beryl::throw_arg_read_error("Bervenv does not exist to remove");
    std::error_code ec;
    bool do_rem = false;
    if (argc > 2) {
      if (std::string(argv[2]) == "--force") do_rem = true;
    } else do_rem = ask();
    if (do_rem && !fs::remove_all("__bervenv__", ec))
      beryl::throw_arg_read_error("Failed to destroy venv: " + ec.message());
  } else if (is_mode("build")) {
    if (!directory_exists("__bervenv__")) beryl::throw_arg_read_error("No bervenv to compile in");
    std::vector<fs::path> paths_to_by_file;
    std::vector<std::string> includes;
    std::optional<fs::path> exec{};
    Version ver{ .major = 1, .minor = 0 };
    bool link = true;
    bool force_module_recompile = false;
    OptLevel opt_level = OptLevel::NONE;

    for (size_t i = 2; i < argc; ++i) {
      std::string arg = argv[i];
      if (fs::path(arg).extension() == ".by") paths_to_by_file.emplace_back(arg);
      else if (arg == "--no-link") link = false;
      else if (arg == "--force-module-recompile") force_module_recompile = true;
      else if (arg == "-O0") opt_level = OptLevel::NONE;
      else if (arg == "-O1") opt_level = OptLevel::ONE;
      else if (arg == "-O2") opt_level = OptLevel::TWO;
      else if (arg == "-O3") opt_level = OptLevel::THREE;
      else if (arg.rfind("-includes=", 0) == 0) includes = beryl::get_includes(arg.substr(10));
      else if (arg.rfind("-out=", 0) == 0) exec = arg.substr(5);
      else if (arg.rfind("-std=", 0) == 0) {
        std::string verstr = arg.substr(5);
        if (verstr == "be1") ver = { .major = 1, .minor = 0 };
        else beryl::throw_arg_read_error("Unknown version: " + verstr);
      } else beryl::throw_arg_read_warning("Unknown compiler argument: " + arg);
    }

    if (paths_to_by_file.size() == 0) beryl::throw_arg_read_error("There is no .by file to compile");
    if (paths_to_by_file.size() > 1 && exec.has_value())
      beryl::throw_arg_read_error("Cannot redirect output for multiple files");

    llvm::LLVMContext context;

    if (ver.major == 1) {
      for (const std::filesystem::path& path : paths_to_by_file) {
        llvm::Module mod("BerylliumModule", context);
        llvm::IRBuilder<> builder(context);

        std::string buf;
        if (!fs::exists(path)) {
          bool found = false;
          for (const auto& prefix : includes) {
            if (auto appended_path = prefix / path; fs::exists(appended_path)) {
              std::ifstream file(appended_path);
              std::ostringstream tmp;
              tmp << file.rdbuf();
              buf = tmp.str();
              found = true;
              break;
            }
          }
          if (!found) beryl::throw_arg_read_error("Could not find file: " + path.string());
        } else {
          std::ifstream file(path);
          std::ostringstream tmp;
          tmp << file.rdbuf();
          buf = tmp.str();
        }

        std::vector<Token> tokens = [&buf, &path]() {
          std::vector<Token> tokens;
          size_t cursor = 0;
          int line = 1;
          int col = 1;

          auto peek = [&](size_t offset = 0) {
            if (cursor + offset >= buf.length()) return '\0';
            return buf[cursor + offset];
          };

          while (cursor < buf.length()) {
            //
          }
          return tokens;
        }();
      }
    }
  } else beryl::throw_arg_read_error("Invalid option");
  return 0;
}
