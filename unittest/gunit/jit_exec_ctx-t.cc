#include <assert.h>
#include <gtest/gtest.h>
#include <stddef.h>
#include <iostream>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "sql/jit/jit_exec_ctx.h"

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

#include "llvm/Support/TargetSelect.h"

class JITItemCompiledTests : public testing::Test {
 protected:
 public:
};

TEST_F(JITItemCompiledTests, SimpleCompile) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  auto llvm_context = std::make_unique<llvm::LLVMContext>();

  // Open a new module
  auto llvm_module = std::make_unique<llvm::Module>("test_jit", *llvm_context);
  llvm_module->setDataLayout(jit_exec_ctx->get_data_layout());

  // Create an ew builder for the module
  auto llvm_builder = std::make_unique<llvm::IRBuilder<>>(*llvm_context);

  // Create function
  auto return_type = llvm::Type::getInt32Ty(*llvm_context);
  llvm::FunctionType *functionType =
      llvm::FunctionType::get(return_type, false);
  llvm::Function *function = llvm::Function::Create(
      functionType, llvm::Function::ExternalLinkage, "__main", *llvm_module);

  // Set entry point
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(*llvm_context, "entry", function);
  llvm_builder->SetInsertPoint(entry);

  llvm::Value *a =
      llvm::ConstantInt::get(*llvm_context, llvm::APInt(32, 2, true));
  llvm::Value *b =
      llvm::ConstantInt::get(*llvm_context, llvm::APInt(32, 4, true));

  llvm::Value *sum = llvm_builder->CreateAdd(a, b, "a+b");
  llvm_builder->CreateRet(sum);

  // Compile function
  auto tsm = llvm::orc::ThreadSafeModule(std::move(llvm_module),
                                         std::move(llvm_context));
  auto err = jit_exec_ctx->add_module(std::move(tsm));

  auto exprSymbol = jit_exec_ctx->lookup("__main").get();

  auto compiled_func = (int (*)())(intptr_t)exprSymbol.getAddress();
  ASSERT_TRUE(compiled_func != nullptr);

  ASSERT_TRUE(compiled_func() == 6);
};
