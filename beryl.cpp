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
    STR_LIT,
    CHAR_LIT,
    VAR,
    IDENT,
    IF,
    WHILE,
    DO,
    COLON,
    COMMA,
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
    ASSIGN,
    NOT_EQ,
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
    VALAT,
    PTRTO,
    SIZEOF
  } type;

  std::string metadata;
  int line;
  int col;
};

const std::unordered_map<std::string_view, decltype(Token::VAR)> keywords = {
    {";", Token::SEMI},
    {"var", Token::VAR},
    {"if", Token::IF},
    {"while", Token::WHILE},
    {"do", Token::DO},
    {":", Token::COLON},
    {",", Token::COMMA},
    {"func", Token::FUNC},
    {"(", Token::OPEN_PAREN},
    {")", Token::CLOSE_PAREN},
    {"exit", Token::EXIT},
    {"return", Token::RETURN},
    {"+", Token::PLUS},
    {"-", Token::MINUS},
    {"*", Token::ASTER},
    {"/", Token::FORW_SLASH},
    {"\\", Token::BACK_SLASH},
    {"%", Token::MOD},
    {"mut", Token::MUT},
    {"<<", Token::LEFT_SHIFT},
    {">>", Token::RIGHT_SHIFT},
    {"<", Token::LESS},
    {">", Token::GREATER},
    {"{", Token::OPEN_CURLY},
    {"}", Token::CLOSE_CURLY},
    {"for", Token::FOR},
    {"repeat", Token::REPEAT},
    {"band", Token::BAND},
    {"bor", Token::BOR},
    {"bxor", Token::BXOR},
    {"bxnot", Token::BXNOT},
    {"as", Token::AS},
    {"unsafe", Token::UNSAFE},
    {"&&", Token::AND},
    {"||", Token::OR},
    {"!", Token::NOT},
    {"=", Token::ASSIGN},
    { "==", Token::EQ },
    {"!=", Token::NOT_EQ},
    {"bool", Token::BOOL},
    {"char", Token::CHAR},
    {"byte", Token::BYTE},
    {"int", Token::INT},
    {"long", Token::LONG},
    {"qint", Token::QINT},
    {"ubyte", Token::UBYTE},
    {"uint", Token::UINT},
    {"ulong", Token::ULONG},
    {"uqint", Token::UQINT},
    {"float", Token::FLOAT},
    {"double", Token::DOUBLE},
    {"namespace", Token::NMSP},
    {"string", Token::STRING},
    {"bytestream", Token::BYTESTREAM},
    {"arr", Token::ARR},
    {"dynarr", Token::DYNARR},
    {"adr", Token::ADR},
    {"ptr", Token::PTR},
    {"rcptr", Token::RCPTR},
    {"weakptr", Token::WEAKPTR},
    {"unsafeptr", Token::UNSAFEPTR},
    {"ref", Token::REF},
    {"result", Token::RESULT},
    {"tup", Token::TUP},
    {"hexc", Token::HEXC},
    {"unic", Token::UNIC},
    {"typeof", Token::TYPEOF},
    {"valat", Token::VALAT},
    {"ptrto", Token::PTRTO},
    {"sizeof", Token::SIZEOF}
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
    if (do_rem) {
      std::uintmax_t count = fs::remove_all("__bervenv__", ec);
      if (count == static_cast<std::uintmax_t>(-1) || ec.value() != 0)
        beryl::throw_arg_read_error("Failed to destroy venv: " + ec.message());
    }
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

    if (ver == Version{ .major = 1, .minor = 0 }) {
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

        std::vector<Token> tokens = [&buf, &path]() {
          std::vector<Token> tokens;
          size_t cursor = 0;
          int line = 1;
          int col = 1;
          bool in_type_dec = false;

          auto peek = [&](size_t offset = 0) {
            if (cursor + offset >= buf.length())
              return '\0';
            return buf[cursor + offset];
          };

          constexpr int MAX_ITER = 10000;
          int iter_num = 0;
          while (cursor < buf.length() && iter_num < MAX_ITER) {
            // skips whitespace
            while (std::isspace(peek())) {
              std::cerr << "Skipping whitespace\n";
              if (peek() == '\n') ++line;
              ++col;
              ++cursor;
            }
            // checks single-line comments
            if (peek() == '/' && peek(1) == '/') {
              std::cerr << "Saw single-line comment\n";
              cursor += 2;
              col += 2;
              while (peek() != '\n') {
                std::cerr << "Continuing comment\n";
                ++cursor;
                ++col;
              }
              ++cursor;
              ++line;
            }

            // checks multi-line comments
            if (peek() == '/' && peek(1) == '*') {
              std::cerr << "Saw multi-line comment\n";
              cursor += 2;
              col += 2;
              while (!(peek() == '*' && peek(1) == '/') && cursor < buf.length()) {
                std::cerr << "Continued multi-line comment\n";
                if (peek() == '\n') ++line;
                ++cursor;
                ++col;
              }
            }

            // checks string literals (sorry no f strings yet)
            if (peek() == '"') {
              std::cerr << "Saw string literal\n";
              Token tok;
              tok.type = Token::STR_LIT;
              tok.line = line;
              tok.col = col;
              ++cursor;
              ++col;
              while (peek() != '"' && cursor < buf.length()) {
                std::cerr << "Continued string literal\n";
                if (peek() == '\\' && peek(1) == 'n') {
                  cursor += 2;
                  col += 2;
                  tok.metadata += '\n';
                } else if (peek() == '\\' && peek(1) == '\n') {
                  cursor += 2;
                  col += 2;
                  ++line;
                } else if (peek() == '\\' && peek(1) == '\0') {
                  cursor += 2;
                  col += 2;
                  tok.metadata += '\0';
                } else if (peek() == '\\' && peek(1) == '"') {
                  cursor += 2;
                  col += 2;
                  tok.metadata += '"';
                } else {
                  tok.metadata += peek();
                  ++cursor;
                  ++col;
                }
              }
              ++cursor;
              ++col;
              tokens.push_back(tok);
            }

            // checks character
            if (peek() == '\'') {
              // atriv you do this
            }

            // Keywords/Identifiers
            if (std::isalpha(peek()) || peek() == '_') {
              std::string s;
              int start_col = col;
              while (std::isalnum(peek()) || peek() == '_') {
                s += peek();
                ++cursor; ++col;
              }

              Token tok;
              tok.line = line;
              tok.col = start_col;
              tok.metadata = s;

              if (auto it = keywords.find(s); it != keywords.cend()) tok.type = it->second;
              else tok.type = Token::IDENT;

              tokens.push_back(tok);
              continue;
            }

            // Number literals
            if (peek() == '-' || std::isdigit(peek())) {
              // Atriv does this
            }
            ++iter_num;
          }
          return tokens;
        }();
      }
    }
  } else if (is_mode("help") || is_mode("--help")) {
    std::cout << "Beryl help page\n"
      "beryl create: creates a virtual environment\n"
      "    --no-stdlib: does not install the standard library\n"
      "beryl destroy: removes the virtual environment\n"
      "    --force: removes the prompt\n"
      "beryl build: builds a file\n"
      "    --no-link: does not link the object file\n"
      "    --force-module-recompile: recompiles all modules\n"
      "    -O0: no optimization\n"
      "    -O1: one level optimization\n"
      "    -O2: two levels of optimization\n"
      "    -O3: three levels of optimization\n"
      "    -includes= : include paths provided in a JSON file\n"
      "    -out= : output file, with no linking, object file, with linking, executable\n"
      "    -std= : Beryllium standard to use. Valid values are be1. Defaults to latest version, currently be1\n";
  } else if (is_mode("--version") || is_mode("version") || is_mode("-v")) {
    std::cout << "Beryl 1.0\n";
  } else beryl::throw_arg_read_error("Unknown mode");
  return 0;
}
