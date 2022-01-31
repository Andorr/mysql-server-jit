#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

namespace jit {
llvm::Value *codegen_item_func_eq(Item_func_eq *item,
                                  jit::JITBuilderContext *context) {
  Item **items = item->arguments();

  llvm::Value *item_a_value = items[0]->codegen(context);
  if (!item_a_value) {
    return nullptr;
  }

  llvm::Value *item_b_value = items[1]->codegen(context);
  if (!item_b_value) {
    return nullptr;
  }

  llvm::Value *cmp =
      context->builder->CreateICmpEQ(item_a_value, item_b_value, "int = int");
  return context->builder->CreateIntCast(
      cmp, llvm::Type::getInt64Ty(*context->context),
      false);  // TODO: Verify this bool
}
}  // namespace jit
