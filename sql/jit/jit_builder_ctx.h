#ifndef JIT_BUILDER_CTX_H
#define JIT_BUILDER_CTX_H

#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"

namespace jit {

class JITBuilderContext {
 public:
  std::unique_ptr<llvm::Module> func_module;
  std::unique_ptr<llvm::IRBuilder<>> builder;
  std::unique_ptr<llvm::LLVMContext> context;
  std::unique_ptr<llvm::FunctionPassManager> pass_manager;

  llvm::Function *main_function;

 public:
  ~JITBuilderContext() {
    if (func_module != nullptr) {
      auto ptr = func_module.release();
      delete ptr;
    }
  }
};
}  // namespace jit

#endif