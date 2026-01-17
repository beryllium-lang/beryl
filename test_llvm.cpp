#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include <iostream>

int main() {
  llvm::LLVMContext context;
  llvm::Module module("test_module", context);
  llvm::IRBuilder<> builder(context);

  std::cout << "LLVM module created successfully!" << std::endl;
}
