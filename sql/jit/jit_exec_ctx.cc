#include "memory"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"

#include "jit_exec_ctx.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> JITExecutionContext::new_exec_context() {
  return nullptr;
}