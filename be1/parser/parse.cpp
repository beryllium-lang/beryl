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
      log("starting the construction on funcitons");
      return func;
    }

    ast::NamespaceDecl* construct_nsp(TokenStream& tokens, Arena& arena) {
      auto nsp = arena.alloc<ast::NamespaceDecl>();
      tokens.advance();
      if (!tokens.match(Token::IDENT)) {
        throw_lex_error("a namespace must have a name", tokens.peek(1).line, tokens.peek(1).col);
      } else nsp->name = tokens.peek().metadata;
      while (tokens.peek() != Token::CLOSE_BRACKET) {
        // TODO: Implement
      }
      return nsp;
    }

    ast::VarDecl* construct_var(TokenStream& tokens, Arena& arena) {
      auto var = arena.alloc<ast::VarDecl>();

      // variable thingy
      if (tokens.peek() == Token::PUBLISH) {
        //var->publish = true;
        tokens.advance();
        log("variable published whatever that means");
      }
      tokens.advance();

      // variable name
      if (tokens.peek() != Token::IDENT) {
        throw_lex_error("a variable must have a name", tokens.peek().line, tokens.peek().col);
        
      } else {
        var->name = tokens.peek().metadata;
        tokens.advance();
        
      }

      // now the colon and equals to difference
      if (tokens.peek() == Token::COLON) {
        tokens.advance();
        log("colon detected");
        if (tokens.peek() == Token::IDENT) {
          log("btw the metadata is this, do some stuff with it" + tokens.peek().metadata);
          throw_lex_error("did not implement type logic yet, but easier, so implement ",tokens.peek().line,tokens.peek().col);
      } else if (tokens.peek() == Token::ASSIGN) {
        // this logic is harder, have to identify varialbe type
        throw_lex_error("did not implement variable definition logic yet", tokens.peek().line, tokens.peek().col);
      } else {
        throw_lex_error("variable must be defined", tokens.peek().line, tokens.peek().col);
      }
      
      return var;
    }
    }
    ast::ImportDecl* construct_import(TokenStream& tokens, Arena& arena) {
      auto import = arena.alloc<ast::ImportDecl>();
      
      /* if (tokens.peek() == Token::PUBLISH) {
        import->publish = true;
        tokens.advance();
      } else import->publish = false;
      tokens.advance();
      auto next_token = tokens.peek();
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
        if (prev_is_dot && curr_tok == Token::DOT) {
          throw_lex_error("invalid module name", curr_tok.line, curr_tok.col);
        } else if (curr_tok == Token::IDENT) {
          if (!prev_is_dot) throw_lex_error("invalid module name", curr_tok.line, curr_tok.col);
          import->module_name += curr_tok.metadata;
          prev_is_dot = false;
        } else if (curr_tok == Token::DOT) {
          import->module_name += '.';
          prev_is_dot = true;
        } else if (curr_tok == Token::FROM) {
          if (prev_is_dot) throw_lex_error("invalid module name", curr_tok.line, curr_tok.col);
          throw_lex_error("did not implement from logic yet", curr_tok.line, curr_tok.col);
          tokens.advance();
        } else if (curr_tok == Token::SEMI) {
          if (prev_is_dot) throw_lex_error("invalid module name", curr_tok.line, curr_tok.col);
          tokens.advance();
          break;
        } else throw_lex_error("unexpected token: " + curr_tok.to_string(), curr_tok.line, curr_tok.col);
      } */
      
      if (tokens.peek() == Token::PUBLISH) {
        import->publish = true;
        tokens.advance();
      } else import->publish = false;
      tokens.advance();

      if (tokens.peek() != Token::IDENT && tokens.peek() != Token::DOT) {
        throw_lex_error("a module name must come after an import statement", tokens.peek().line, tokens.peek().col);
      }

      if (tokens.peek() == Token::DOT) {
        tokens.advance();
        if (tokens.peek() == Token::IDENT) {
          import->module_name = tokens.peek().metadata;
          tokens.advance();
          if (tokens.peek() == Token::DOT) {
            throw_lex_error("bro im too lazy to do this, just stop doing stuff wiht beryllium", tokens.peek().line, tokens.peek().col);
          }
        } else {
          throw_lex_error("a module name must come after an import statement", tokens.peek().line, tokens.peek().col);
        }
      } else if (tokens.peek() == Token::IDENT) {
        import->module_name = tokens.peek().metadata;
        tokens.advance();
        bool module_name_detected = false; 
        if (tokens.peek() == Token::SEMI) {
            module_name_detected = true; 
        }
        while (!module_name_detected) { // hopefully i did this correctly
          if (tokens.peek() == Token::DOT) {
            import->module_name += ".";
            tokens.advance();
            if (tokens.peek() == Token::SEMI) {
              throw_lex_error("module name cannot end on a dot", tokens.peek().line, tokens.peek().col);
            }
          } else if (tokens.peek() == Token::IDENT) {
            import->module_name += tokens.peek().metadata;
            tokens.advance();
            if (tokens.peek() == Token::SEMI) {
              log("yup i found the import statements");
              module_name_detected = true;
              break;
            }
          } else {
            throw_lex_error("unexpected keyword at", tokens.peek().line, tokens.peek().col);
          }
        }
      }
      log("exitted the import contructer mess");
      tokens.advance();
      return import;
    }

    ast::EnumDecl* construct_enum(TokenStream& tokens, Arena& arena) {
      auto enum_ = arena.alloc<ast::EnumDecl>();
      if (tokens.match(Token::UNSAFE)) enum_->is_unsafe = true;

      if (!tokens.match(Token::IDENT)) {
        throw_lex_error("an enum must have a name", tokens.peek(1).line, tokens.peek(1).col);
      } else {
        enum_->enum_name = tokens.peek().metadata;
      }
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
