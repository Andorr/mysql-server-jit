#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

namespace jit {
llvm::Value *codegen_item_func_between(Item_func_between *item,
                                       jit::JITBuilderContext *context) {
  Item **items = item->arguments();

  llvm::Value *item_to_cmp = jit::codegen_item(items[0], context);
  if (!item_to_cmp) {
    return nullptr;
  }

  llvm::Value *from_value = jit::codegen_item(items[1], context);
  if (!from_value) {
    return nullptr;
  }

  llvm::Value *to_value = jit::codegen_item(items[2], context);
  if (!to_value) {
    return nullptr;
  }

  llvm::Value *ge =
      context->builder->CreateICmpSGE(item_to_cmp, from_value, "int >= int");
  llvm::Value *le =
      context->builder->CreateICmpSLE(item_to_cmp, to_value, "int <= int");
  llvm::Value *item_between = context->builder->CreateAnd(ge, le);

  return context->builder->CreateIntCast(
      item_between, llvm::Type::getInt64Ty(*context->context), false);
}

}  // namespace jit
