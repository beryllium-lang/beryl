#include "utils/error.hpp"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <filesystem>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
  std::vector<fs::path> paths_to_by_file;
  bool link = true;
  bool force_module_recompile = false;
  for (size_t i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (fs::path(arg).extension() == ".by")
      paths_to_by_file.emplace_back(arg);
    else if (arg == "--no-link")
      link = false;
    else if (arg == "--force-module-recompile")
      force_module_recompile = false;
  }

  if (paths_to_by_file.size() == 0)
    beryl::throw_compiler_error("There is no .by file to compile\n");

  llvm::LLVMContext context;
  llvm::Module module("BerylliumModule", context);
  llvm::IRBuilder<> builder(context);
  return 0;
}
