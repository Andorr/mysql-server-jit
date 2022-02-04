#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

namespace jit {
llvm::Value *codegen_item_field(Item_field *item,
                                jit::JITBuilderContext *context) {
  llvm::Value *row_ptr = context->builder->CreateIntCast(
      llvm::ConstantInt::get(
          *context->context,
          llvm::APInt(64, (uint64_t)&item->field->table->record[0])),
      context->builder->getInt64Ty(), true, "row_pointer");

  llvm::Value *row_start_ptr =
      context->builder->CreateLoad(llvm::Type::getInt64PtrTy(*context->context),
                                   row_ptr, "load_row_start_ptr");

  auto field_offset = item->field->offset(item->field->table->record[0]);

  llvm::Value *field_offset_value =
      llvm::ConstantInt::get(*context->context, llvm::APInt(64, field_offset));
  llvm::Value *row_column_ptr = context->builder->CreateAdd(
      row_start_ptr, field_offset_value, "row_column_ptr");

  llvm::Value *column_value =
      context->builder->CreateLoad(llvm::Type::getInt64Ty(*context->context),
                                   row_column_ptr, "column_value");

  return column_value;
}
}  // namespace jit
