#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

#define CMPFUNC(cmp_func)                                               \
  Item **arguments = item->arguments();                                 \
  llvm::Value *item_a_value = jit::codegen_item(arguments[0], context); \
  if (!item_a_value) {                                                  \
    return nullptr;                                                     \
  }                                                                     \
  llvm::Value *item_b_value = jit::codegen_item(arguments[1], context); \
  if (!item_b_value) {                                                  \
    return nullptr;                                                     \
  }                                                                     \
  llvm::Value *cmp = cmp_func(item_a_value, item_b_value, "int > int"); \
  return context->builder->CreateIntCast(                               \
      cmp, llvm::Type::getInt64Ty(*context->context),                   \
      false);  // TODO: Verify this bool

namespace jit {

llvm::Value *codegen_item_func_gt(Item_func_gt *item,
                                  jit::JITBuilderContext *context){
    CMPFUNC(context->builder->CreateICmpSGT)}

llvm::Value *codegen_item_func_eq(Item_func_eq *item,
                                  jit::JITBuilderContext *context){
    CMPFUNC(context->builder->CreateICmpEQ)}

llvm::Value *codegen_item_func_lt(Item_func_lt *item,
                                  jit::JITBuilderContext *context){
    CMPFUNC(context->builder->CreateICmpSLT)}

llvm::Value *codegen_item_func_ge(Item_func_ge *item,
                                  jit::JITBuilderContext *context){
    CMPFUNC(context->builder->CreateICmpSGE)}

llvm::Value *codegen_item_func_le(Item_func_le *item,
                                  jit::JITBuilderContext *context) {
  CMPFUNC(context->builder->CreateICmpSLE)
}

}  // namespace jit