#include "sql/item.h"
#include "sql/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

namespace jit {
llvm::Value *codegen_item_int(Item_int *item, jit::JITBuilderContext *context) {
  return llvm::ConstantInt::get(*context->context,
                                llvm::APInt(64, (uint64_t)item->value));
}

}  // namespace jit