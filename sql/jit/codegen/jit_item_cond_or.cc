#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

namespace jit {
llvm::Value *codegen_item_cond_or(Item_cond_or *item,
                                  jit::JITBuilderContext *context) {
  Item **items = item->arguments();

  llvm::Value *item_a_value = jit::codegen_item(items[0], context);
  if (!item_a_value) {
    return nullptr;
  }

  llvm::Value *item_b_value = jit::codegen_item(items[1], context);
  if (!item_b_value) {
    return nullptr;
  }

  llvm::Value *item_or =
      context->builder->CreateOr(item_a_value, item_b_value, "int or int");
  return context->builder->CreateIntCast(
      item_or, llvm::Type::getInt64Ty(*context->context), false);
}
}  // namespace jit
