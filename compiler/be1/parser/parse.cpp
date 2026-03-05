#include "parse.hpp"
#include "utils/error.hpp"

namespace beryl::be1 {
  namespace {
    ast::VarDecl* construct_var(TokenStream&, Arena&);

    ast::ClassDecl* construct_class(TokenStream& tokens, Arena& arena) {
      auto class_ = arena.alloc<ast::ClassDecl>();
      tokens.advance();
      if (auto tok = tokens.peek(1); tok != Token::IDENT) beryl::throw_lex_error("class does not have identifier", tok.line, tok.col);
      else class_->name = tok.metadata;
      tokens.advance();
      if (auto tok = tokens.peek(1); tok != Token::OPEN_CURLY) beryl::throw_lex_error("class not opened with brace", tok.line, tok.col);
      tokens.advance();
      return class_;
    }

    ast::FunctionDecl* construct_func(TokenStream& tokens, Arena& arena) {
      auto func = arena.alloc<ast::FunctionDecl>();
      return func;
    }

    ast::NamespaceDecl* construct_nsp(TokenStream& tokens, Arena& arena) {
      auto nsp = arena.alloc<ast::NamespaceDecl>();
      return nsp;
    }

    ast::VarDecl* construct_var(TokenStream& tokens, Arena& arena) {
      auto var = arena.alloc<ast::VarDecl>();
      return var;
    }

    ast::ImportDecl* construct_import(TokenStream& tokens, Arena& arena) {
      auto import = arena.alloc<ast::ImportDecl>();
      if (tokens.peek() == Token::PUBLISH) {
        import->publish = true;
        tokens.advance();
      } else import->publish = false;
      auto next_token = tokens.peek(1);
      if (next_token != Token::IDENT && next_token != Token::DOT) {
        throw_lex_error("a module name must come after an import statement", next_token.line, next_token.col);
      }
      tokens.advance();
      bool prev_is_dot = next_token == Token::DOT;
      if (prev_is_dot) {
        import->module_name += '.';
        tokens.advance();
      }
      while (true) {
        const auto& curr_tok = tokens.peek();
        if (const auto& tok = tokens.peek(1); tok != Token::SEMI && tok != Token::FROM)
          throw_lex_error("invalid import statement", tok.line, tok.col);
        if ((curr_tok == Token::DOT && prev_is_dot) || (curr_tok == Token::IDENT && !prev_is_dot))
          throw_lex_error("invalid module name", curr_tok.line, curr_tok.col);
        if (curr_tok == Token::DOT) {
        }
        if (curr_tok == Token::IDENT) import->module_name += curr_tok.metadata;
      }
      tokens.advance();
      return import;
    }

    ast::EnumDecl* construct_enum(TokenStream& tokens, Arena& arena) {
      auto enum_ = arena.alloc<ast::EnumDecl>();
      return enum_;
    }

    ast::TraitDecl* construct_trait(TokenStream& tokens, Arena& arena) {
      auto trait = arena.alloc<ast::TraitDecl>();
      return trait;
    }
  } // namespace

  ast::Program* parse(TokenStream& tokens, Arena& arena) {
    ast::Program* tree = arena.alloc<ast::Program>();
    while (tokens.has_next() && tokens.peek().type != Token::EOF_TOKEN) {
      if (tokens.peek() == Token::CLASS) tree->body.emplace_back(construct_class(tokens, arena));
      else if (tokens.peek() == Token::FUNC || (tokens.peek() == Token::PUBLISH && tokens.peek(1) == Token::FUNC))
        tree->body.emplace_back(construct_func(tokens, arena));
      else if (tokens.peek() == Token::NMSP) tree->body.emplace_back(construct_nsp(tokens, arena));
      else if (tokens.peek() == Token::VAR || (tokens.peek() == Token::PUBLISH && tokens.peek(1) == Token::VAR))
        tree->body.emplace_back(construct_var(tokens, arena));
      else if (tokens.peek() == Token::IMPORT || (tokens.peek() == Token::PUBLISH && tokens.peek(1) == Token::IMPORT))
        tree->body.emplace_back(construct_import(tokens, arena));
      else if (tokens.peek() == Token::ENUM) tree->body.emplace_back(construct_enum(tokens, arena));
      else if (tokens.peek() == Token::TRAIT) tree->body.emplace_back(construct_trait(tokens, arena));
      else beryl::throw_lex_error("unknown global-level declaration", tokens.peek().line, tokens.peek().col);
      continue;
    }
    return tree;
  }
} // namespace beryl::be1