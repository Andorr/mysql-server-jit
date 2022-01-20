#ifndef JIT_BUILDER_CTX_H
#define JIT_BUILDER_CTX_H

namespace jit {

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"

class JITBuilderContext {
  llvm::Value *value;

  std::unique_ptr<llvm::Module> functionModule;
  std::unique_ptr<llvm::IRBuilder<>> builder;
  std::unique_ptr<llvm::LLVMContext> context;
  std::unique_ptr<llvm::FunctionPassManager> passManager;

  llvm::Function *mainFunction;

  ~JITBuilderContext() {
    if (functionModule != nullptr) {
      auto ptr = functionModule.release();
      delete ptr;
    }
  }
};
}  // namespace jit

#endif