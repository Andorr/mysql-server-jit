#ifndef JIT_INCLUDED
#define JIT_INCLUDED

// class Item {
//  public:
//   std::optional<bool> canCompileCache = std::nullopt;
//
//  public:
//   virtual bool canCompile() { return false; };
//   virtual llvm::Value *codegen(JITExecutionContext *context) {
//     return nullptr;
//   };
// };

#include "memory"
#include "optional"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"
namespace jit {

#include "jit_builder_ctx.h"
#include "jit_exec_ctx.h"

using namespace jit;

class Item;

std::optional<std::unique_ptr<JITExecutionContext>> new_jit_exec_ctx();
std::optional<int> compile_where_cond(
    std::unique_ptr<JITExecutionContext> *jit_exec_ctx, Item *where_cond);

};  // namespace jit

#endif