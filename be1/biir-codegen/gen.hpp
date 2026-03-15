#pragma once
#include "be1/parser/parse.hpp"

namespace beryl::be1 {
  std::string ast_to_biir(const ast::Program* const prog);
}