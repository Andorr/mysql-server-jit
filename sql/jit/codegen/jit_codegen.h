#ifndef JIT_CODEGEN_H
#define JIT_CODEGEN_H

#include "llvm/IR/Value.h"
#include "sql/jit/jit_builder_ctx.h"

class Item;
class Item_int;
class Item_cache_int;
class Item_func_eq;
class Item_func_gt;
class Item_func_lt;
class Item_field;
class Item_cond_or;
class Item_func_between;
class Item_func_ge;
class Item_func_le;
class Item_func_like;

namespace jit {

llvm::Value *codegen_item(Item *item, jit::JITBuilderContext *context);

llvm::Value *codegen_item_int(Item_int *item, jit::JITBuilderContext *context);
llvm::Value *codegen_item_cache_int(Item_cache_int *item,
                                    jit::JITBuilderContext *context);
llvm::Value *codegen_item_func_eq(Item_func_eq *item,
                                  jit::JITBuilderContext *context);
llvm::Value *codegen_item_field(Item_field *item,
                                jit::JITBuilderContext *context);

llvm::Value *codegen_item_cond_or(Item_cond_or *item,
                                  jit::JITBuilderContext *context);

llvm::Value *codegen_item_func_between(Item_func_between *item,
                                       jit::JITBuilderContext *context);

llvm::Value *codegen_item_func_le(Item_func_le *item,
                                  jit::JITBuilderContext *context);

llvm::Value *codegen_item_func_ge(Item_func_ge *item,
                                  jit::JITBuilderContext *context);
llvm::Value *codegen_item_func_gt(Item_func_gt *item,
                                  jit::JITBuilderContext *context);

llvm::Value *codegen_item_func_lt(Item_func_lt *item,
                                  jit::JITBuilderContext *context);
llvm::Value *codegen_item_func_like(Item_func_like *item,
                                    jit::JITBuilderContext *context);

}  // namespace jit

#endif
