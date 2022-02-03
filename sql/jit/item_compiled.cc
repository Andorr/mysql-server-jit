#include <memory>

#include "jit_common.h"
#include "sql/jit/codegen/jit_codegen.h"
#include "sql/jit/item_compiled.h"

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

  // We always return an 64-bit integer
  auto return_type = llvm::Type::getInt64Ty(*builder_ctx->context);
  llvm::FunctionType *functionType =
      llvm::FunctionType::get(return_type, false);

  llvm::Function *function =
      llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                             "__main", *builder_ctx->func_module);
  builder_ctx->main_function = function;

  // Set entry point
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(*builder_ctx->context, "entry", function);
  builder_ctx->builder->SetInsertPoint(entry);

  llvm::Value *value = jit::codegen_item(this->item, builder_ctx.get());
  builder_ctx->builder->CreateRet(value);
}

void Item_compiled::jit_compile(jit::JITExecutionContext *exec_ctx) {
  llvm::ExitOnError exit_on_err;
  auto TSM = llvm::orc::ThreadSafeModule(std::move(builder_ctx->func_module),
                                         std::move(builder_ctx->context));

  // TODO: Replace exit_on_err with better error-handling
  if (auto err = exec_ctx->add_module(std::move(TSM))) {
    // debug_print("error: was not able to add module");
    return;
  }

  // TODO: Replace exit_on_err with better error-handling
  if (auto expr_symbol = exec_ctx->lookup("__main")) {
    this->compiled_func =
        std::make_unique<uint64_t>((uint64_t)(*expr_symbol).getAddress());
  } else {
    // debug_print("error: was not able to lookup __main");
  }
}

longlong Item_compiled::val_int() {
  assert(this->compiled_func != nullptr);

  int_func_ptr func = reinterpret_cast<int_func_ptr>(*this->compiled_func);
  return func();
}

void Item_compiled::print_ir() {
  builder_ctx->main_function->print(llvm::errs());
  fprintf(stderr, "\n");
}
/*
void print(const THD *thd, String *str,
             enum_query_type query_type) const override {

             } */