#pragma once
#include <string>
#include <string_view>
#include <vector>

namespace beryl::be1 {
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
      DOT,
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
      NMSP,
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
      OPT,
      UNIC,
      TYPEOF,
      VALAT,
      PTRTO,
      SIZEOF,
      PLUS_EQ,
      MINUS_EQ,
      ASTER_EQ,
      FORW_SLASH_EQ,
      MOD_EQ,
      PLUS_PLUS,
      MINUS_MINUS,
      IMPORT,
      FROM,
      TRUE,
      FALSE,
      NULLPTR,
      UNKNOWN_TOKEN,
      OPEN_BRACKET,
      CLOSE_BRACKET,
      ELSE,
      EOF_TOKEN,
      CLASS,
      PUBLIC,
      PRIVATE,
      OP,
      FRIEND,
      TYPE,
      TRAIT,
      COMB
    } type;

    std::string metadata;
    int line;
    int col;

    bool operator==(const Token& other) const {
      return type == other.type && metadata == other.metadata;
    }
    bool operator!=(const Token& other) const { return !(*this == other); }
    bool operator==(decltype(Token::VAR) t) const { return type == t; }
    bool operator!=(decltype(Token::VAR) t) const { return type != t; }

    std::string to_string() const {
      switch (type) {
      case SEMI: return "SEMI";
      case INT_LIT: return "INT_LIT(" + metadata + ")";
      case FLOAT_LIT: return "FLOAT_LIT(" + metadata + ")";
      case STR_LIT: return "STR_LIT(" + metadata + ")";
      case CHAR_LIT: return "CHAR_LIT(" + metadata + ")";
      case VAR: return "VAR";
      case IDENT: return "IDENT(" + metadata + ")";
      case IF: return "IF";
      case WHILE: return "WHILE";
      case DO: return "DO";
      case COLON: return "COLON";
      case COMMA: return "COMMA";
      case DOT: return "DOT";
      case FUNC: return "FUNC";
      case OPEN_PAREN: return "OPEN_PAREN";
      case CLOSE_PAREN: return "CLOSE_PAREN";
      case EXIT: return "EXIT";
      case RETURN: return "RETURN";
      case PLUS: return "PLUS";
      case MINUS: return "MINUS";
      case ASTER: return "ASTER";
      case FORW_SLASH: return "FORW_SLASH";
      case BACK_SLASH: return "BACK_SLASH";
      case MOD: return "MOD";
      case MUT: return "MUT";
      case LEFT_SHIFT: return "LEFT_SHIFT";
      case LESS: return "LESS";
      case GREATER: return "GREATER";
      case OPEN_CURLY: return "OPEN_CURLY";
      case CLOSE_CURLY: return "CLOSE_CURLY";
      case FOR: return "FOR";
      case REPEAT: return "REPEAT";
      case BAND: return "BAND";
      case BOR: return "BOR";
      case BXOR: return "BXOR";
      case BXNOT: return "BXNOT";
      case AS: return "AS";
      case UNSAFE: return "UNSAFE";
      case AND: return "AND";
      case OR: return "OR";
      case NOT: return "NOT";
      case EQ: return "EQ";
      case ASSIGN: return "ASSIGN";
      case NOT_EQ: return "NOT_EQ";
      case BOOL: return "BOOL";
      case CHAR: return "CHAR";
      case BYTE: return "BYTE";
      case INT: return "INT";
      case LONG: return "LONG";
      case QINT: return "QINT";
      case UBYTE: return "UBYTE";
      case UINT: return "UINT";
      case ULONG: return "ULONG";
      case UQINT: return "UQINT";
      case FLOAT: return "FLOAT";
      case DOUBLE: return "DOUBLE";
      case NMSP: return "NMSP";
      case STRING: return "STRING";
      case BYTESTREAM: return "BYTESTREAM";
      case ARR: return "ARR";
      case DYNARR: return "DYNARR";
      case ADR: return "ADR";
      case PTR: return "PTR";
      case RCPTR: return "RCPTR";
      case WEAKPTR: return "WEAKPTR";
      case UNSAFEPTR: return "UNSAFEPTR";
      case REF: return "REF";
      case RESULT: return "RESULT";
      case TUP: return "TUP";
      case OPT: return "OPT";
      case UNIC: return "UNIC";
      case TYPEOF: return "TYPEOF";
      case VALAT: return "VALAT";
      case PTRTO: return "PTRTO";
      case SIZEOF: return "SIZEOF";
      case PLUS_EQ: return "PLUS_EQ";
      case MINUS_EQ: return "MINUS_EQ";
      case ASTER_EQ: return "ASTER_EQ";
      case FORW_SLASH_EQ: return "FORW_SLASH_EQ";
      case MOD_EQ: return "MOD_EQ";
      case PLUS_PLUS: return "PLUS_PLUS";
      case MINUS_MINUS: return "MINUS_MINUS";
      case UNKNOWN_TOKEN: return "UNKNOWN_TOKEN";
      case IMPORT: return "IMPORT";
      case FROM: return "FROM";
      case TRUE: return "TRUE";
      case FALSE: return "FALSE";
      case NULLPTR: return "NULLPTR";
      case CLASS: return "CLASS";
      case PUBLIC: return "PUBLIC";
      case PRIVATE: return "PRIVATE";
      case OP: return "OP";
      case TYPE: return "TYPE";
      case TRAIT: return "TRAIT";
      case COMB: return "COMB";
      case EOF_TOKEN: return "EOF_TOKEN";
      default: return "INVALID_TOKEN";
      }
    }
  };

  using TokenType = decltype(Token::VAR);
  class TokenStream {
  private:
    std::vector<Token> tokens;
    size_t cursor;

  public:
    TokenStream() : cursor(0) {}
    TokenStream(const std::vector<Token>& tokens_) : tokens(tokens_), cursor(0) {}

    bool has_next() const { return cursor < tokens.size(); }
    const Token& peek(size_t offset = 0) const;
    const Token& advance();
    bool match(TokenType type);

    class Iterator {
    private:
      const std::vector<Token>* data;
      size_t cursor;

      explicit Iterator(const TokenStream& stream) : data(&stream.tokens), cursor(stream.cursor) {}

    public:
      Iterator() = delete;

      const Token& operator*() const { return (*data)[cursor]; }
      const Token* operator->() const { return &operator*(); }

      Iterator& operator++() {
        ++cursor;
        return *this;
      }

      Iterator operator++(int) {
        Iterator copy(*this);
        ++cursor;
        return copy;
      }

      Iterator look_ahead(size_t offset_) const {
        Iterator res(*this);
        res.cursor += offset_;
        return res;
      }

      Iterator& operator+=(size_t offset_) {
        cursor += offset_;
        return *this;
      }

      bool operator==(const Iterator& other) const {
        return other.data == data && other.cursor == cursor;
      }
      bool operator!=(const Iterator& other) const { return !operator==(other); }

      friend TokenStream;
    };

    Iterator begin() { return Iterator(*this); }
    Iterator end() { return begin().look_ahead(tokens.size() - cursor); }
  };

  TokenStream lex(std::string_view source, std::string_view path);
} // namespace beryl::be1