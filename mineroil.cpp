#include "utils/error.hpp"
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include "third-party/json.hpp"

int main(int argc, char* argv[]) {
  std::cout << std::boolalpha;
  std::unordered_map<std::string, int> x;
  x["hello"] = 9;
  return 0;
}