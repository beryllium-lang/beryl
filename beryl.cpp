#include "utils/error.hpp"
#include "utils/parse_json_params.hpp"
#include "utils/arena.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <optional>

struct CompileArgs {
  int argc;
  char** argv;
};

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
    SEMI, INT_LIT,
    FLOAT_LIT, STR_LIT,
    CHAR_LIT, VAR,
    IDENT, IF,
    WHILE, DO,
    COLON, COMMA,
    DOT, FUNC,
    OPEN_PAREN, CLOSE_PAREN,
    EXIT, RETURN,
    PLUS, MINUS,
    ASTER, FORW_SLASH,
    BACK_SLASH, MOD,
    MUT, LEFT_SHIFT,
    LESS, GREATER,
    OPEN_CURLY, CLOSE_CURLY,
    FOR, REPEAT,
    BAND, BOR,
    BXOR, BXNOT,
    AS, UNSAFE,
    AND, OR,
    NOT, EQ,
    ASSIGN, NOT_EQ,
    BOOL, CHAR,
    BYTE, INT,
    LONG, QINT,
    UBYTE, UINT,
    ULONG, UQINT,
    FLOAT, DOUBLE,
    NMSP, STRING,
    BYTESTREAM, ARR,
    DYNARR, ADR,
    PTR, RCPTR,
    WEAKPTR, UNSAFEPTR,
    REF, RESULT,
    TUP, HEXC,
    UNIC, TYPEOF,
    VALAT, PTRTO,
    SIZEOF, PLUS_EQ,
    MINUS_EQ, ASTER_EQ,
    FORW_SLASH_EQ, MOD_EQ,
    PLUS_PLUS, MINUS_MINUS,
    IMPORT, FROM,
    TRUE, FALSE,
    NULLPTR, UNKNOWN_TOKEN,
    EOF_TOKEN
  } type;

  std::string metadata;
  int line;
  int col;

  std::string to_string() const {
    switch (type) {
      case SEMI: return "SEMI"; case INT_LIT: return "INT_LIT";
      case FLOAT_LIT: return "FLOAT_LIT"; case STR_LIT: return "STR_LIT";
      case CHAR_LIT: return "CHAR_LIT"; case VAR: return "VAR";
      case IDENT: return "IDENT(" + metadata + ")"; case IF: return "IF";
      case WHILE: return "WHILE"; case DO: return "DO";
      case COLON: return "COLON"; case COMMA: return "COMMA";
      case DOT: return "DOT"; case FUNC: return "FUNC";
      case OPEN_PAREN: return "OPEN_PAREN"; case CLOSE_PAREN: return "CLOSE_PAREN";
      case EXIT: return "EXIT"; case RETURN: return "RETURN";
      case PLUS: return "PLUS"; case MINUS: return "MINUS";
      case ASTER: return "ASTER"; case FORW_SLASH: return "FORW_SLASH";
      case BACK_SLASH: return "BACK_SLASH"; case MOD: return "MOD";
      case MUT: return "MUT"; case LEFT_SHIFT: return "LEFT_SHIFT";
      case LESS: return "LESS"; case GREATER: return "GREATER";
      case OPEN_CURLY: return "OPEN_CURLY"; case CLOSE_CURLY: return "CLOSE_CURLY";
      case FOR: return "FOR"; case REPEAT: return "REPEAT";
      case BAND: return "BAND"; case BOR: return "BOR";
      case BXOR: return "BXOR"; case BXNOT: return "BXNOT";
      case AS: return "AS"; case UNSAFE: return "UNSAFE";
      case AND: return "AND"; case OR: return "OR";
      case NOT: return "NOT"; case EQ: return "EQ";
      case ASSIGN: return "ASSIGN"; case NOT_EQ: return "NOT_EQ";
      case BOOL: return "BOOL"; case CHAR: return "CHAR";
      case BYTE: return "BYTE"; case INT: return "INT";
      case LONG: return "LONG"; case QINT: return "QINT";
      case UBYTE: return "UBYTE"; case UINT: return "UINT";
      case ULONG: return "ULONG"; case UQINT: return "UQINT";
      case FLOAT: return "FLOAT"; case DOUBLE: return "DOUBLE";
      case NMSP: return "NMSP"; case STRING: return "STRING";
      case BYTESTREAM: return "BYTESTREAM"; case ARR: return "ARR";
      case DYNARR: return "DYNARR"; case ADR: return "ADR";
      case PTR: return "PTR"; case RCPTR: return "RCPTR";
      case WEAKPTR: return "WEAKPTR"; case UNSAFEPTR: return "UNSAFEPTR";
      case REF: return "REF"; case RESULT: return "RESULT";
      case TUP: return "TUP"; case HEXC: return "HEXC";
      case UNIC: return "UNIC"; case TYPEOF: return "TYPEOF";
      case VALAT: return "VALAT"; case PTRTO: return "PTRTO";
      case SIZEOF: return "SIZEOF"; case PLUS_EQ: return "PLUS_EQ";
      case MINUS_EQ: return "MINUS_EQ"; case ASTER_EQ: return "ASTER_EQ";
      case FORW_SLASH_EQ: return "FORW_SLASH_EQ"; case MOD_EQ: return "MOD_EQ";
      case PLUS_PLUS: return "PLUS_PLUS"; case MINUS_MINUS: return "MINUS_MINUS";
      case UNKNOWN_TOKEN: return "UNKNOWN_TOKEN"; case IMPORT: return "IMPORT";
      case FROM: return "FROM"; case TRUE: return "TRUE";
      case FALSE: return "FALSE"; case NULLPTR: return "NULLPTR";
      case EOF_TOKEN: return "EOF_TOKEN";
      default: return "INVALID_TOKEN";
    }
  }
};

using TokenType = decltype(Token::VAR);
struct TokenStream {
private:
  std::vector<Token> tokens;
  size_t cursor;
public:
  explicit TokenStream() : cursor(0) {}
  explicit TokenStream(std::vector<Token>&& toks) : tokens(std::move(toks)), cursor(0) {}
  explicit TokenStream(const std::vector<Token>& toks) : tokens(toks), cursor(0) {}

  bool has_next() const { return cursor < tokens.size(); }

  const Token& peek(size_t offset = 0) const {
    if (cursor + offset >= tokens.size()) return tokens.back();
    return tokens[cursor + offset];
  }

  const Token& advance() {
    if (!has_next()) beryl::throw_lex_error("End of token stream reached", -1, -1);
    return tokens[cursor++];
  }

  bool match(TokenType type) {
    if (has_next() && peek().type == type) {
      advance();
      return true;
    }
    return false;
  }

  std::vector<Token>::iterator begin() { return tokens.begin(); }
  std::vector<Token>::iterator end() { return tokens.end(); }
  std::vector<Token>::const_iterator cbegin() const { return tokens.cbegin(); }
  std::vector<Token>::const_iterator cend() const { return tokens.cend(); }
};

const std::unordered_map<std::string_view, TokenType> keywords = {
    {"var", Token::VAR}, {"if", Token::IF},
    {"while", Token::WHILE}, {"do", Token::DO},
    {"func", Token::FUNC}, {"exit", Token::EXIT},
    {"return", Token::RETURN}, {"mut", Token::MUT},
    {"for", Token::FOR}, {"repeat", Token::REPEAT},
    {"band", Token::BAND}, {"bor", Token::BOR},
    {"bxor", Token::BXOR}, {"bxnot", Token::BXNOT},
    {"as", Token::AS}, {"unsafe", Token::UNSAFE},
    {"bool", Token::BOOL}, {"char", Token::CHAR},
    {"byte", Token::BYTE}, {"int", Token::INT},
    {"long", Token::LONG}, {"qint", Token::QINT},
    {"ubyte", Token::UBYTE}, {"uint", Token::UINT},
    {"ulong", Token::ULONG}, {"uqint", Token::UQINT},
    {"float", Token::FLOAT}, {"double", Token::DOUBLE},
    {"namespace", Token::NMSP}, {"string", Token::STRING},
    {"bytestream", Token::BYTESTREAM}, {"arr", Token::ARR},
    {"dynarr", Token::DYNARR}, {"adr", Token::ADR},
    {"ptr", Token::PTR}, {"rcptr", Token::RCPTR},
    {"weakptr", Token::WEAKPTR}, {"unsafeptr", Token::UNSAFEPTR},
    {"ref", Token::REF}, {"result", Token::RESULT},
    {"tup", Token::TUP}, {"hexc", Token::HEXC},
    {"unic", Token::UNIC}, {"typeof", Token::TYPEOF},
    {"valat", Token::VALAT}, {"ptrto", Token::PTRTO},
    {"sizeof", Token::SIZEOF}, {"import", Token::IMPORT},
    {"from", Token::FROM}, {"true", Token::TRUE},
    {"false", Token::FALSE}, {"nullptr", Token::NULLPTR} };

const std::unordered_map<char, TokenType> symbols = {
    {';', Token::SEMI},        {'(', Token::OPEN_PAREN},
    {')', Token::CLOSE_PAREN}, {'{', Token::OPEN_CURLY},
    {'}', Token::CLOSE_CURLY}, {':', Token::COLON},
    {',', Token::COMMA},       {'+', Token::PLUS},
    {'-', Token::MINUS},       {'*', Token::ASTER},
    {'/', Token::FORW_SLASH},  {'\\', Token::BACK_SLASH},
    {'%', Token::MOD},         {'<', Token::LESS},
    {'>', Token::GREATER},     {'=', Token::ASSIGN},
    {'!', Token::NOT},         {'.', Token::DOT} };

const std::unordered_map<std::string, TokenType> double_symbols = {
    {"&&", Token::AND},
    {"||", Token::OR},
    {"==", Token::EQ},
    {"!=", Token::NOT_EQ},
    {"<<", Token::LEFT_SHIFT},
    {"+=", Token::PLUS_EQ},
    {"-=", Token::MINUS_EQ},
    {"*=", Token::ASTER_EQ},
    {"/=", Token::FORW_SLASH_EQ},
    {"%=", Token::MOD_EQ},
    {"++", Token::PLUS_PLUS},
    {"--", Token::MINUS_MINUS} };

namespace fs = std::filesystem;

bool ask() {
  std::cout << "Are you sure? Y/n: ";
  char c;
  std::cin >> c;
  return c == 'Y' || c == 'y';
}

bool directory_exists(const fs::path& path) {
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
      std::system("python3 -m mineraloil install stdlib");
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

#pragma region AST Nodes



#pragma endregion

void build(CompileArgs args) {
  beryl::Arena alloc;
  if (!directory_exists("__bervenv__")) {
    std::cerr << "Bervenv does not exist. Would you like to create it? Y/n: ";
    char c;
    std::cin >> c;
    if (c == 'Y' || c == 'y') {
      create_venv(args);
    } else beryl::fail();
  }
  std::vector<fs::path> paths_to_by_file;
  std::vector<std::string> includes;
  std::optional<fs::path> exec{};
  Version ver{ .major = 1, .minor = 0 };
  bool link = true;
  bool force_module_recompile = false;
  OptLevel opt_level = OptLevel::NONE;

  for (size_t i = 2; i < args.argc; ++i) {
    std::string arg = args.argv[i];
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

      TokenStream tokens([&buf, &path, &alloc]() {
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

        size_t past_cursor = cursor;
        while (cursor < buf.length()) {
          // skips whitespace
          if (std::isspace(peek())) {
            if (peek() == '\n') {
              ++line;
              ++cursor;
              col = 1;
            } else {
              ++cursor;
              ++col;
            }
            continue;
          }
          // checks single-line comments
          if (peek() == '/' && peek(1) == '/') {
            cursor += 2;
            col += 2;
            while (peek() != '\n' && peek() != '\0') {
              ++cursor;
              ++col;
            }
            if (peek() != '\0') {
              ++cursor;
              ++line;
            }
            continue;
          }

          // checks multi-line comments
          if (peek() == '/' && peek(1) == '*') {
            cursor += 2;
            col += 2;
            while (!(peek() == '*' && peek(1) == '/') && cursor < buf.length()) {
              if (peek() == '\n')
                ++line;
              ++cursor;
              ++col;
            }
            if (peek() == '*' && peek(1) == '/') {
              cursor += 2;
              col += 2;
            }
            continue;
          }

          // checks string literals (sorry no f strings yet)
          if (peek() == '"') {
            Token tok;
            tok.type = Token::STR_LIT;
            tok.line = line;
            tok.col = col;
            ++cursor;
            ++col;
            while (peek() != '"' && cursor < buf.length()) {
              if (peek() == '\\' && peek(1) == 'n') {
                cursor += 2;
                col += 2;
                tok.metadata += '\n';
              } else if (peek() == '\\' && peek(1) == '\n') {
                cursor += 2;
                col += 2;
                ++line;
              } else if (peek() == '\\' && peek(1) == '0') {
                cursor += 2;
                col += 2;
                tok.metadata += '\0';
              } else if (peek() == '\\' && peek(1) == '"') {
                cursor += 2;
                col += 2;
                tok.metadata += '"';
              } else if (peek() != '\\') {
                tok.metadata += peek();
                ++cursor;
                ++col;
              } else {
                beryl::throw_lex_error("Unknown escape sequence in string literal", line, col);
              }
            }
            if (cursor >= buf.length()) {
              beryl::throw_lex_error("Unterminated string literal", tok.line, tok.col);
            }
            ++cursor;
            ++col;
            tokens.push_back(tok);
            continue;
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
              ++cursor;
              ++col;
            }

            Token tok;
            tok.line = line;
            tok.col = start_col;
            tok.metadata = s;

            if (auto it = keywords.find(s); it != keywords.cend())
              tok.type = it->second;
            else
              tok.type = Token::IDENT;

            tokens.push_back(tok);
            continue;
          }

          // Number literals
          if ((peek() == '-' && std::isdigit(peek(1))) || std::isdigit(peek())) {
            Token tok;
            tok.line = line;
            tok.col = col;
            std::string num_str;
            bool is_float = false;
            if (peek() == '-') {
              num_str += '-';
              ++cursor;
              ++col;
            }
            while (std::isdigit(peek()) || peek() == '.') {
              if (peek() == '.') {
                if (is_float) {
                  beryl::throw_lex_error(
                    "Multiple decimal points in number literal", line, col);
                }
                is_float = true;
              }
              num_str += peek();
              ++cursor;
              ++col;
            }
            tok.type = is_float ? Token::FLOAT_LIT : Token::INT_LIT;
            tok.metadata = num_str;
            tokens.push_back(tok);
            continue;
          }

          // Symbols
          bool matched_symbol = false;
          if (cursor + 1 < buf.length()) {
            std::string potential_double = { peek(), peek(1) };
            if (potential_double == ">>") {
              for (int i = 0; i < 2; ++i) {
                Token tok;
                tok.type = Token::GREATER;
                tok.line = line;
                tok.col = col + i;
                tok.metadata = ">";
                tokens.push_back(tok);
              }
              cursor += 2;
              col += 2;
              matched_symbol = true;
            }
            if (auto it = double_symbols.find(potential_double); it != double_symbols.cend()) {
              Token tok;
              tok.type = it->second;
              tok.line = line;
              tok.col = col;
              tok.metadata = potential_double;
              tokens.push_back(tok);
              cursor += 2;
              col += 2;
              matched_symbol = true;
            }
          }

          if (!matched_symbol) {
            if (auto it = symbols.find(peek()); it != symbols.cend()) {
              Token tok;
              tok.type = it->second;
              tok.line = line;
              tok.col = col;
              tok.metadata = peek();
              tokens.push_back(tok);
              ++cursor;
              ++col;
              matched_symbol = true;
            } else {
              beryl::throw_lex_error(
                std::string("Unknown symbol: ") + peek(), line, col);
            }
          }

          if (matched_symbol) {
            continue;
          }

          if (cursor == past_cursor) {
            beryl::throw_lex_error("Tokenizer stopped progress", line, col);
          }
          past_cursor = cursor;
        }
        tokens.push_back(Token{ .type = Token::EOF_TOKEN, .line = line, .col = col });
        return tokens;
      }());
      for (const auto& token : tokens) {
        std::cout << token.to_string() << " at line " << token.line << ", col " << token.col
          << "\n";
      }
    }
  }
}

void help() {
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
}

int main(int argc, char* argv[]) {
  CompileArgs args{ argc, argv };
  std::cout << std::boolalpha;
  auto is_mode = [argv](std::string_view str) { return str == argv[1]; };
  if (argc == 1)
    beryl::throw_arg_read_error(
      "Can choose beryl build, beryl create, or beryl destroy");
  if (is_mode("create")) {
    create_venv(args);
  } else if (is_mode("destroy")) {
    destroy_venv(args);
  } else if (is_mode("build")) {
    build(args);
  } else if (is_mode("help") || is_mode("--help")) {
    help();
  } else if (is_mode("--version") || is_mode("version") || is_mode("-v")) {
    std::cout << "Beryl 1.0\n";
  } else beryl::throw_arg_read_error("Unknown mode");
  return 0;
}
