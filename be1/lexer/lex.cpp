#include "lex.hpp"
#include "utils/error.hpp"
#include <unordered_map>

namespace beryl::be1 {
  static const std::unordered_map<std::string_view, TokenType> KEYWORDS = {
      {"var", Token::VAR},
      {"if", Token::IF},
      {"while", Token::WHILE},
      {"do", Token::DO},
      {"func", Token::FUNC},
      {"exit", Token::EXIT},
      {"return", Token::RETURN},
      {"mut", Token::MUT},
      {"for", Token::FOR},
      {"repeat", Token::REPEAT},
      {"band", Token::BAND},
      {"bor", Token::BOR},
      {"bxor", Token::BXOR},
      {"bxnot", Token::BXNOT},
      {"as", Token::AS},
      {"unsafe", Token::UNSAFE},
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
      {"opt", Token::OPT},
      {"unic", Token::UNIC},
      {"typeof", Token::TYPEOF},
      {"valat", Token::VALAT},
      {"ptrto", Token::PTRTO},
      {"sizeof", Token::SIZEOF},
      {"import", Token::IMPORT},
      {"from", Token::FROM},
      {"true", Token::TRUE},
      {"false", Token::FALSE},
      {"nullptr", Token::NULLPTR},
      {"else", Token::ELSE},
      {"trait", Token::TRAIT}};

  static const std::unordered_map<char, TokenType> SYMBOLS = {
      {';', Token::SEMI},          {'(', Token::OPEN_PAREN},  {'[', Token::OPEN_BRACKET},
      {']', Token::CLOSE_BRACKET}, {')', Token::CLOSE_PAREN}, {'{', Token::OPEN_CURLY},
      {'}', Token::CLOSE_CURLY},   {':', Token::COLON},       {',', Token::COMMA},
      {'+', Token::PLUS},          {'-', Token::MINUS},       {'*', Token::ASTER},
      {'/', Token::FORW_SLASH},    {'\\', Token::BACK_SLASH}, {'%', Token::MOD},
      {'<', Token::LESS},          {'>', Token::GREATER},     {'=', Token::ASSIGN},
      {'!', Token::NOT},           {'.', Token::DOT},         {'|', Token::COMB}};

  static const std::unordered_map<std::string_view, TokenType> DOUBLE_SYMBOLS = {
      {"&&", Token::AND},      {"||", Token::OR},         {"==", Token::EQ},
      {"!=", Token::NOT_EQ},   {"<<", Token::LEFT_SHIFT}, {"+=", Token::PLUS_EQ},
      {"-=", Token::MINUS_EQ}, {"*=", Token::ASTER_EQ},   {"/=", Token::FORW_SLASH_EQ},
      {"%=", Token::MOD_EQ},   {"++", Token::PLUS_PLUS},  {"--", Token::MINUS_MINUS}};

  const Token& TokenStream::advance() {
    if (!has_next()) beryl::throw_lex_error("End of token stream reached", -1, -1);
    return tokens[cursor++];
  }

  const Token& TokenStream::peek(size_t offset) const {
    if (cursor + offset >= tokens.size()) return tokens.back();
    return tokens[cursor + offset];
  }

  bool TokenStream::match(TokenType type) {
    if (has_next() && peek().type == type) {
      advance();
      return true;
    }
    return false;
  }

  TokenStream lex(std::string_view buf, std::string_view path) {
    std::vector<Token> tokens;
    size_t cursor = 0;
    int line = 1;
    int col = 1;

    auto peek = [&](size_t offset = 0) {
      if (cursor + offset >= buf.length()) return '\0';
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
          if (peek() == '\n') {
            ++line;
            ++cursor;
            col = 1;
          } else {
            ++cursor;
            ++col;
          }
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
        Token tok;
        tok.type = Token::CHAR_LIT;
        tok.line = line;
        tok.col = col;
        ++cursor;
        ++col;
        if (peek() == '\\') {
          if (peek(1) == 'n') {
            tok.metadata = '\n';
            cursor += 2;
            col += 2;
          } else if (peek(1) == '0') {
            tok.metadata = '\0';
            cursor += 2;
            col += 2;
          } else if (peek(1) == '\'') {
            tok.metadata = '\'';
            cursor += 2;
            col += 2;
          } else {
            beryl::throw_lex_error("Unknown escape sequence in character literal", line, col);
          }
        } else {
          tok.metadata = peek();
          ++cursor;
          ++col;
        }
        if (peek() != '\'') {
          beryl::throw_lex_error("Unterminated character literal", tok.line, tok.col);
        }
        ++cursor;
        ++col;
        tokens.push_back(tok);
        continue;
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

        if (auto it = KEYWORDS.find(s); it != KEYWORDS.cend())
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
              beryl::throw_lex_error("Multiple decimal points in number literal", line, col);
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
        std::string potential_double{peek(), peek(1)};
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
        if (auto it = DOUBLE_SYMBOLS.find(potential_double); it != DOUBLE_SYMBOLS.cend()) {
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
        if (auto it = SYMBOLS.find(peek()); it != SYMBOLS.cend()) {
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
          beryl::throw_lex_error(std::string("Unknown symbol: ") + peek(), line, col);
        }
      }

      if (matched_symbol) { continue; }

      if (cursor == past_cursor) {
        beryl::throw_lex_error("Tokenizer stopped progress", line, col);
      }
      past_cursor = cursor;
    }
    tokens.push_back(Token{.type = Token::EOF_TOKEN, .line = line, .col = col});
    return TokenStream(tokens);
  }
} // namespace beryl::be1