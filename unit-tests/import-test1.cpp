#include "be1/parser/parse.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <gtest/gtest.h>

TEST(ImportTest, ImportTest1) {
  using namespace beryl;

  {
    std::ofstream out_file("h.txt");
    std::streambuf* orig = std::cout.rdbuf();
    std::cout.rdbuf(out_file.rdbuf());

    std::string test = R"(import std;
import std.io;
import .local;
import .local.sub)";
    
    Arena arena(128 * 1024 * 1024);

    be1::TokenStream stream = be1::lex(test, "internal unit test string");

    auto tree = be1::parse(stream, arena);

    std::cout.flush();
    std::cout.rdbuf(orig);
  } 

  std::ifstream in_file("h.txt");
  std::string msg(std::istreambuf_iterator<char>(in_file), {});
  if (msg.find("error: ") != std::string::npos) {
    FAIL() << "There should not be errors\n";
  }
}
