#ifndef JIT_INCLUDED
#define JIT_INCLUDED

#include "llvm/IR/Value.h"

class JITBuilderContext {
  llvm::Value *value;
};

class JITExecutionContext {
  llvm::Value *value;
};

#endif