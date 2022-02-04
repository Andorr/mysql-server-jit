#ifndef JIT_CODEGEN_H
#define JIT_CODEGEN_H

#include "llvm/IR/Value.h"
#include "sql/jit/jit_builder_ctx.h"

class Item;
class Item_int;
class Item_func_eq;
class Item_func_gt;
class Item_func_lt;
class Item_field;

namespace jit {

llvm::Value *codegen_item(Item *item, jit::JITBuilderContext *context);

llvm::Value *codegen_item_int(Item_int *item, jit::JITBuilderContext *context);
llvm::Value *codegen_item_func_eq(Item_func_eq *item,
                                  jit::JITBuilderContext *context);
llvm::Value *codegen_item_field(Item_field *item,
                                jit::JITBuilderContext *context);

llvm::Value *codegen_item_func_gt(Item_func_gt *item,
                                  jit::JITBuilderContext *context);

llvm::Value *codegen_item_func_lt(Item_func_gt *item,
                                  jit::JITBuilderContext *context);

}  // namespace jit

#endif
