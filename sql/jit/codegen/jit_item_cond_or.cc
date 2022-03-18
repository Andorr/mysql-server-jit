#include <vector>

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

  llvm::Value *output = nullptr;
  if (list_count == 1) {
    output = jit::codegen_item(list[0], context);
  } else {
    llvm::Value *item_a_value = jit::codegen_item(list[0], context);
    if (!item_a_value) {
      return nullptr;
    }

    llvm::Value *item_b_value = jit::codegen_item(list[1], context);
    if (!item_b_value) {
      return nullptr;
    }

    llvm::Value *item_or =
        context->builder->CreateOr(item_a_value, item_b_value);

    for (uint i = 2; i < list_count; i++) {
      llvm::Value *item_i_value = jit::codegen_item(list[i], context);
      item_or = context->builder->CreateOr(item_or, item_i_value);
    }

    output = item_or;
  }

  return context->builder->CreateIntCast(
      output, llvm::Type::getInt64Ty(*context->context), false);
}

llvm::Value *codegen_item_cond_and(Item_cond_and *item,
                                   jit::JITBuilderContext *context) {
  auto list = *item->argument_list();
  auto list_count = list.size();

  llvm::Value *output = nullptr;
  if (list_count == 1) {
    output = jit::codegen_item(list[0], context);
  } else {
    llvm::Function *func = context->builder->GetInsertBlock()->getParent();

    llvm::BasicBlock *end_bb =
        llvm::BasicBlock::Create(*context->context, "end_and");

    std::vector<llvm::BasicBlock *> basic_blocks;
    std::vector<llvm::Value *> block_values;

    basic_blocks.push_back(context->builder->GetInsertBlock());

    for (uint i = 0; i < list_count; i++) {
      llvm::BasicBlock *then_bb =
          llvm::BasicBlock::Create(*context->context, "then");
      llvm::Value *then_v = jit::codegen_item(list[i], context);
      if (then_v == nullptr) {
        return nullptr;
      }

      // context->builder->CreateCondBr(then_v, then_bb, end_bb);

      if (i == list_count - 1) {
        context->builder->CreateBr(end_bb);
        block_values.push_back(then_v);
        // basic_blocks.push_back(end_bb);
      } else {
        context->builder->CreateCondBr(then_v, then_bb, end_bb);
        func->getBasicBlockList().push_back(then_bb);
        context->builder->SetInsertPoint(then_bb);
        basic_blocks.push_back(then_bb);
        block_values.push_back(llvm::ConstantInt::get(
            *context->context, llvm::APInt(64, 0, true)));
      }
    }

    // Codegen for MERGE
    func->getBasicBlockList().push_back(end_bb);
    context->builder->SetInsertPoint(end_bb);
    llvm::PHINode *PN = context->builder->CreatePHI(
        llvm::Type::getInt64Ty(*context->context), 2, "itmp");

    for (size_t i = 0; i < block_values.size(); i++) {
      PN->addIncoming(block_values[i], basic_blocks[i]);
    }

    output = PN;
  }

  return output;
}
}  // namespace jit
