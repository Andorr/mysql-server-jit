#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

namespace jit {
llvm::Value *codegen_item_func_le(Item_func_le *item,
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

  llvm::Value *item_le =
      context->builder->CreateICmpSLE(item_a_value, item_b_value, "int <= int");
  return context->builder->CreateIntCast(
      item_le, llvm::Type::getInt64Ty(*context->context), false);
}

}  // namespace jit
