#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/item_func.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

#define CMPFUNC(cmp_func)                                                  \
  Item **arguments = item->arguments();                                    \
  Item *a = arguments[0];                                                  \
  Item *b = arguments[1];                                                  \
                                                                           \
  if ((a->type() == Item::Type::FIELD_ITEM &&                              \
       b->type() == Item::Type::STRING_ITEM) ||                            \
      (a->type() == Item::Type::STRING_ITEM &&                             \
       b->type() == Item::Type::FIELD_ITEM) ||                             \
      (a->type() == Item::Type::STRING_ITEM &&                             \
       b->type() == Item::Type::STRING_ITEM)) {                            \
    llvm::FunctionType *function_type =                                    \
        llvm::FunctionType::get(context->builder->getInt64Ty(), false);    \
                                                                           \
    auto val_int_ptr = (item->functype() != Item_func::Functype::EQ_FUNC   \
                            ? (uint64_t)&item_val_int                      \
                            : (uint64_t)&item_cmp);                        \
    llvm::Value *func_ptr = llvm::ConstantInt::get(                        \
        *context->context, llvm::APInt(64, (val_int_ptr)));                \
    llvm::Value *item_ptr = llvm::ConstantInt::get(                        \
        *context->context, llvm::APInt(64, (uint64_t)item));               \
                                                                           \
    auto call_inst = context->builder->CreateCall(                         \
        function_type, func_ptr, llvm::ArrayRef<llvm::Value *>{item_ptr}); \
    return context->builder->CreateIntCast(                                \
        call_inst, llvm::Type::getInt64Ty(*context->context), false);      \
  }                                                                        \
  llvm::Value *item_a_value = jit::codegen_item(arguments[0], context);    \
  if (!item_a_value) {                                                     \
    return nullptr;                                                        \
  }                                                                        \
  llvm::Value *item_b_value = jit::codegen_item(arguments[1], context);    \
  if (!item_b_value) {                                                     \
    return nullptr;                                                        \
  }                                                                        \
                                                                           \
  llvm::Value *cmp = cmp_func(item_a_value, item_b_value, "int > int");    \
  return context->builder->CreateIntCast(                                  \
      cmp, llvm::Type::getInt64Ty(*context->context), false);

static longlong item_val_int(Item *item) { return item->val_int(); }

static longlong item_cmp(Item_bool_func2 *item) {
  auto value = item->get_comparator()->compare();
  return value == 0 ? 1 : 0;
}

namespace jit {

llvm::Value *codegen_item_func_like(Item_func_like *item,
                                    jit::JITBuilderContext *context){
    CMPFUNC(context->builder->CreateICmpEQ)}

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
                                  jit::JITBuilderContext *context){
    CMPFUNC(context->builder->CreateICmpSLE)}

llvm::Value *codegen_item_func_ne(Item_func_ne *item,
                                  jit::JITBuilderContext *context){
    CMPFUNC(context->builder->CreateICmpNE)}

llvm::Value *codegen_item_func_not(Item_func_not *item,
                                   jit::JITBuilderContext *context) {
  Item **arguments = item->arguments();
  llvm::Value *item_a_value = jit::codegen_item(arguments[0], context);
  if (!item_a_value) {
    return nullptr;
  }
  llvm::Value *i1 = context->builder->CreateIntCast(
      item_a_value, llvm::Type::getInt1Ty(*context->context), false);
  llvm::Value *cmp = context->builder->CreateNot(i1, "not int");
  return context->builder->CreateIntCast(
      cmp, llvm::Type::getInt64Ty(*context->context), false);
}

}  // namespace jit
