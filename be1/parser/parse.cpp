#include "parse.hpp"
#include "utils/error.hpp"

namespace beryl::be1 {
  namespace {
    struct ArenaWrapper {
      Arena& arena;

      template <typename T> T* alloc() {
        T* res = arena.alloc<T>();
        if (!res) throw_arg_read_error("Ran out of space in arena");
        return res;
      }
    };

    ast::VarDecl* construct_var(TokenStream&, ArenaWrapper&);

    ast::ClassDecl* construct_class(TokenStream& tokens, ArenaWrapper& arena) {
      auto class_ = arena.alloc<ast::ClassDecl>();
      tokens.advance();
      if (auto tok = tokens.peek(1); tok != Token::IDENT) beryl::throw_lex_error("Class does not have identifier", tok.line, tok.col);
      else
        class_->name = tok.metadata;
      tokens.advance();
      if (auto tok = tokens.peek(1); tok != Token::OPEN_CURLY) beryl::throw_lex_error("Class not opened with brace", tok.line, tok.col);
      tokens.advance();
      return class_;
    }

    ast::FunctionDecl* construct_func(TokenStream& tokens, ArenaWrapper& arena) {
      auto func = arena.alloc<ast::FunctionDecl>();
      return func;
    }

    ast::NamespaceDecl* construct_nsp(TokenStream& tokens, ArenaWrapper& arena) {
      auto nsp = arena.alloc<ast::NamespaceDecl>();
      return nsp;
    }

    ast::VarDecl* construct_var(TokenStream& tokens, ArenaWrapper& arena) {
      auto var = arena.alloc<ast::VarDecl>();
      return var;
    }

    ast::ImportDecl* construct_import(TokenStream& tokens, ArenaWrapper& arena) {
      auto import = arena.alloc<ast::ImportDecl>();
      return import;
    }

    ast::EnumDecl* construct_enum(TokenStream& tokens, ArenaWrapper& arena) {
      auto enum_ = arena.alloc<ast::EnumDecl>();
      return enum_;
    }

    ast::TraitDecl* construct_trait(TokenStream& tokens, ArenaWrapper& arena) {
      auto trait = arena.alloc<ast::TraitDecl>();
      return trait;
    }
  } // namespace

  ast::Program* parse(TokenStream& tokens, Arena& raw_arena) {
    ArenaWrapper arena{.arena = raw_arena};
    ast::Program* tree = arena.alloc<ast::Program>();
    while (tokens.has_next() && tokens.peek().type != Token::EOF_TOKEN) {
      if (tokens.peek() == Token::CLASS) tree->body.emplace_back(construct_class(tokens, arena));
      else if (tokens.peek() == Token::FUNC || (tokens.peek() == Token::PUBLISH && tokens.peek(1) == Token::FUNC))
        tree->body.emplace_back(construct_func(tokens, arena));
      else if (tokens.peek() == Token::NMSP)
        tree->body.emplace_back(construct_nsp(tokens, arena));
      else if (tokens.peek() == Token::VAR || (tokens.peek() == Token::PUBLISH && tokens.peek(1) == Token::VAR))
        tree->body.emplace_back(construct_var(tokens, arena));
      else if (tokens.peek() == Token::IMPORT || (tokens.peek() == Token::PUBLISH && tokens.peek(1) == Token::IMPORT))
        tree->body.emplace_back(construct_import(tokens, arena));
      else if (tokens.peek() == Token::ENUM)
        tree->body.emplace_back(construct_enum(tokens, arena));
      else if (tokens.peek() == Token::TRAIT)
        tree->body.emplace_back(construct_trait(tokens, arena));
      else
        beryl::throw_lex_error("Unknown global-level declaration", tokens.peek().line, tokens.peek().col);
      continue;
    }
    return tree;
  }
} // namespace beryl::be1