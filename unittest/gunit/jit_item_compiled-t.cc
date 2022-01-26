#include <assert.h>
#include <gtest/gtest.h>
#include <stddef.h>
#include <iostream>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "sql/jit/item_compiled.h"
#include "sql/jit/jit_exec_ctx.h"

#include "unittest/gunit/fake_table.h"
#include "unittest/gunit/test_utils.h"

#include "llvm/Support/TargetSelect.h"

using my_testing::Server_initializer;

class JITItemCompiledTests : public testing::Test {
 protected:
  Server_initializer initializer;

  void SetUp() override { initializer.SetUp(); }

  void TearDown() override { initializer.TearDown(); }

  THD *thd() { return initializer.thd(); }

 public:
};

TEST_F(JITItemCompiledTests, CompileItem) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), nullptr);
  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_TRUE(result == 6);
};
