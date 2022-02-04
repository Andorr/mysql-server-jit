#include "sql/jit/codegen/jit_codegen.h"
#include "sql/jit/jit_builder_ctx.h"

#include "llvm/IR/Value.h"

#include "sql/item.h"
#include "sql/item_cmpfunc.h"

llvm::Value *jit::codegen_item(Item *item, jit::JITBuilderContext *context) {
  if (Item_int *it_int = dynamic_cast<Item_int *>(item)) {
    return jit::codegen_item_int(it_int, context);
  } else if (Item_func_eq *it_func_eq = dynamic_cast<Item_func_eq *>(item)) {
    return jit::codegen_item_func_eq(it_func_eq, context);
  } else if (Item_field *it_field = dynamic_cast<Item_field *>(item)) {
    return jit::codegen_item_field(it_field, context);
  } else if (Item_func_between *it_between =
                 dynamic_cast<Item_func_between *>(item)) {
    return jit::codegen_item_func_between(it_between, context);
  } else if (Item_func_ge *it_ge = dynamic_cast<Item_func_ge *>(item)) {
    return jit::codegen_item_func_ge(it_ge, context);
  } else if (Item_func_le *it_le = dynamic_cast<Item_func_le *>(item)) {
    return jit::codegen_item_func_le(it_le, context);
  } else if (Item_cond_or *ir_or = dynamic_cast<Item_cond_or *>(item)) {
    return jit::codegen_item_cond_or(ir_or, context);
  }
  return nullptr;
}