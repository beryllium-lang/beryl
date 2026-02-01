#include "parse.hpp"

namespace beryl::be1 {
  ast::Program* parse(TokenStream& tokens, Arena& arena) {
    ast::Program* tree = arena.alloc<ast::Program>();
    while (tokens.has_next() && tokens.peek().type != Token::EOF_TOKEN) {
      tokens.advance();
    }
    return tree;
  }
}