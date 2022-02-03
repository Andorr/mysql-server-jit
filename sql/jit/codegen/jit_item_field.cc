#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/codegen/jit_codegen.h"

#include "llvm/IR/Constant.h"
#include "llvm/IR/Value.h"

namespace jit {
llvm::Value *codegen_item_field(Item_field *item,
                                jit::JITBuilderContext *context) {
  // Field offset: item->field->table->s->rowid_field_offset;
  return llvm::ConstantInt::get(*context->context,
                                llvm::APInt(64, (uint64_t)69));
}
}  // namespace jit
