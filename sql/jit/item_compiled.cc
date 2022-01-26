#include <memory>

#include "item_compiled.h"

#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

void Item_compiled::codegen_item() {
  // Create function
  // TODO: Make return type based on the type of Item.
  auto return_type = llvm::Type::getInt32Ty(*builder_ctx->context);
  llvm::FunctionType *functionType =
      llvm::FunctionType::get(return_type, false);

  llvm::Function *function =
      llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                             "__main", *builder_ctx->func_module);

  // Set entry point
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(*builder_ctx->context, "entry", function);
  builder_ctx->builder->SetInsertPoint(entry);

  llvm::Value *a =
      llvm::ConstantInt::get(*builder_ctx->context, llvm::APInt(32, 2, true));
  llvm::Value *b =
      llvm::ConstantInt::get(*builder_ctx->context, llvm::APInt(32, 4, true));

  llvm::Value *sum = builder_ctx->builder->CreateAdd(a, b, "a+b");
  builder_ctx->builder->CreateRet(sum);
}

void Item_compiled::jit_compile(jit::JITExecutionContext *exec_ctx) {
  llvm::ExitOnError exit_on_err;
  auto TSM = llvm::orc::ThreadSafeModule(std::move(builder_ctx->func_module),
                                         std::move(builder_ctx->context));

  // TODO: Replace exit_on_err with better error-handling
  exit_on_err(exec_ctx->add_module(std::move(TSM)));

  // TODO: Replace exit_on_err with better error-handling
  auto exprSymbol = exit_on_err(exec_ctx->lookup("__main"));

  this->compiled_func =
      std::make_unique<uint64_t>((uint64_t)exprSymbol.getAddress());
}

longlong Item_compiled::val_int() {
  assert(this->compiled_func != nullptr);

  int_func_ptr func = reinterpret_cast<int_func_ptr>(*this->compiled_func);
  return func();
}