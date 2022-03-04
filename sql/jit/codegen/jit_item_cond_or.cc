#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

namespace jit {
llvm::Value *codegen_item_cond_or(Item_cond_or *item,
                                  jit::JITBuilderContext *context) {
  auto list = *item->argument_list();
  auto list_count = list.size();

  fprintf(stderr, "OR length: %d\n", list.size());

  llvm::Value *item_a_value = jit::codegen_item(list[0], context);
  if (!item_a_value) {
    return nullptr;
  }

  llvm::Value *item_b_value = jit::codegen_item(list[1], context);
  if (!item_b_value) {
    return nullptr;
  }

  llvm::Value *item_or = context->builder->CreateOr(item_a_value, item_b_value);

  for (uint i = 2; i < list_count; i++) {
    llvm::Value *item_i_value = jit::codegen_item(list[i], context);
    item_or = context->builder->CreateOr(item_or, item_i_value);
  }

  return context->builder->CreateIntCast(
      item_or, llvm::Type::getInt64Ty(*context->context), false);
}

llvm::Value *codegen_item_cond_and(Item_cond_and *item,
                                   jit::JITBuilderContext *context) {
  auto list = *item->argument_list();
  auto list_count = list.size();
  fprintf(stderr, "OR length: %d\n", list.size());

  llvm::Value *item_a_value = jit::codegen_item(list[0], context);
  if (!item_a_value) {
    return nullptr;
  }

  llvm::Value *item_b_value = jit::codegen_item(list[1], context);
  if (!item_b_value) {
    return nullptr;
  }

  llvm::Value *item_or =
      context->builder->CreateAnd(item_a_value, item_b_value);

  for (uint i = 2; i < list_count; i++) {
    llvm::Value *item_i_value = jit::codegen_item(list[i], context);
    item_or = context->builder->CreateAnd(item_or, item_i_value);
  }

  return context->builder->CreateIntCast(
      item_or, llvm::Type::getInt64Ty(*context->context), false);
}
}  // namespace jit
