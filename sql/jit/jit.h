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

#include "sql/iterators/composite_iterators.h"

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"

#include "jit_builder_ctx.h"
#include "jit_exec_ctx.h"

#include "item_compiled.h"

class Item;

namespace jit {

std::unique_ptr<JITExecutionContext> new_jit_exec_ctx();
Item_compiled *create_item_compiled_from_item(THD *thd,
                                              JITExecutionContext *jit_exec_ctx,
                                              Item *item);

void initialize();

extern bool initialized;

};  // namespace jit

#endif