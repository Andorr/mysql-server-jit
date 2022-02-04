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

#include "unittest/gunit/fake_table.h"
#include "unittest/gunit/mock_field_long.h"

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

TEST_F(JITItemCompiledTests, CompileItemInt) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *a = new Item_int(7);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), a);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_TRUE(result == 7);
};

TEST_F(JITItemCompiledTests, CompileItemFuncEq) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *a = new Item_int(7);
  Item *b = new Item_int(8);
  Item *c = new Item_func_eq(a, b);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), c);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_TRUE(result == 0);
};

TEST_F(JITItemCompiledTests, CompileItemFieldOnly) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Mock_field_long field(false);
  Fake_TABLE table(&field);
  uint8 value = 10;
  *(table.record[0] + 1) = value;
  table.in_use = thd();
  field.make_readable();

  Item *b = new Item_field(&field);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), b);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, value);

  value = 69;
  *(table.record[0] + 1) = value;

  result = item->val_int();
  ASSERT_EQ(result, value);
};

TEST_F(JITItemCompiledTests, CompileItemField) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *a = new Item_int(0);

  Mock_field_long field(false);
  Fake_TABLE table(&field);
  table.in_use = thd();
  field.make_readable();

  Item *b = new Item_field(&field);
  Item *c = new Item_func_eq(a, b);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), c);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);
};

TEST_F(JITItemCompiledTests, CompileItemFuncGE) {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *a = new Item_int(10);
  Item *b = new Item_int(5);

  Item *c = new Item_func_ge(a, b);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), c);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *d = new Item_int(10);
  Item *e = new Item_int(10);
  Item *f = new Item_func_ge(d, e);
  item = new Item_compiled(jit_exec_ctx.get(), f);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *g = new Item_int(10);
  Item *h = new Item_int(15);
  Item *i = new Item_func_ge(g, h);
  item = new Item_compiled(jit_exec_ctx.get(), i);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 0);
};