#include <assert.h>
#include <gtest/gtest.h>
#include <stddef.h>
#include <iostream>

#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "m_ctype.h"
#include "sql/sql_lex.h"

#include "sql/jit/item_compiled.h"
#include "sql/jit/jit.h"
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

  void SetUp() override {
    initializer.SetUp();
    jit::initialize();
  }

  void TearDown() override { initializer.TearDown(); }

  THD *thd() { return initializer.thd(); }

 public:
};

TEST_F(JITItemCompiledTests, CompileItemInt) {
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
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *a = new Item_int(7);
  Item *b = new Item_int(8);
  Item *c = new Item_func_eq(a, b);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), c);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_TRUE(result == 0);
};

TEST_F(JITItemCompiledTests, CompileItemFieldOnly) {
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
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, value);

  value = 69;
  *(table.record[0] + 1) = value;

  result = item->val_int();
  ASSERT_EQ(result, value);
};

TEST_F(JITItemCompiledTests, CompileItemField) {
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
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);
};

TEST_F(JITItemCompiledTests, CompileItemFuncGE) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *a = new Item_int(10);
  Item *b = new Item_int(5);

  Item *c = new Item_func_ge(a, b);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), c);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *d = new Item_int(10);
  Item *e = new Item_int(10);
  Item *f = new Item_func_ge(d, e);
  item = new Item_compiled(jit_exec_ctx.get(), f);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *g = new Item_int(10);
  Item *h = new Item_int(15);
  Item *i = new Item_func_ge(g, h);
  item = new Item_compiled(jit_exec_ctx.get(), i);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 0);
};

TEST_F(JITItemCompiledTests, CompileItemFuncLE) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *a = new Item_int(10);
  Item *b = new Item_int(5);

  Item *c = new Item_func_le(a, b);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), c);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 0);

  Item *d = new Item_int(10);
  Item *e = new Item_int(10);
  Item *f = new Item_func_le(d, e);
  item = new Item_compiled(jit_exec_ctx.get(), f);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *g = new Item_int(10);
  Item *h = new Item_int(15);
  Item *i = new Item_func_le(g, h);
  item = new Item_compiled(jit_exec_ctx.get(), i);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 1);
};

TEST_F(JITItemCompiledTests, CompileItemFuncBetween) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *i_10 = new Item_int(10);
  Item *i_5 = new Item_int(5);
  Item *i_7 = new Item_int(7);
  Item *i_11 = new Item_int(11);

  Item *between_a = new Item_func_between(POS(), i_7, i_5, i_10, false);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), between_a);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *between_b = new Item_func_between(POS(), i_5, i_5, i_10, false);
  item = new Item_compiled(jit_exec_ctx.get(), between_b);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *i = new Item_func_between(POS(), i_11, i_5, i_10, false);
  item = new Item_compiled(jit_exec_ctx.get(), i);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 0);
};

TEST_F(JITItemCompiledTests, CompileItemString) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  const char *str_a = "Hello";
  const char *str_b = "Hello";
  const char *str_c = "World :D";

  Item *item_str_a = new Item_string(str_a, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_b = new Item_string(str_b, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_c = new Item_string(str_c, 5, &my_charset_utf8mb4_general_ci);
  Item *item_eq_a = new Item_func_eq(item_str_a, item_str_b);
  item_eq_a->fix_fields(thd(), &item_eq_a);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), item_eq_a);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *item_eq_b = new Item_func_eq(item_str_a, item_str_c);
  item_eq_b->fix_fields(thd(), &item_eq_b);
  item = new Item_compiled(jit_exec_ctx.get(), item_eq_b);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 0);
}

TEST_F(JITItemCompiledTests, CompileItemFuncLike) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  const char *str_a = "Hello";
  const char *str_b = "%Hello%";
  const char *str_c = "YHello world! :D";
  const char *str_d = "Yello world! :D";

  Item *item_str_a = new Item_string(str_a, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_b = new Item_string(str_b, 7, &my_charset_utf8mb4_general_ci);
  Item *item_str_c = new Item_string(str_c, 16, &my_charset_utf8mb4_general_ci);
  Item *item_str_d = new Item_string(str_d, 15, &my_charset_utf8mb4_general_ci);
  Item *item_like_a = new Item_func_like(item_str_c, item_str_b);
  item_like_a->fix_fields(thd(), &item_like_a);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), item_like_a);

  item->codegen_item();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *item_like_b = new Item_func_like(item_str_a, item_str_b);
  item_like_b->fix_fields(thd(), &item_like_b);
  item = new Item_compiled(jit_exec_ctx.get(), item_like_b);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *item_like_c = new Item_func_like(item_str_d, item_str_b);
  item_like_c->fix_fields(thd(), &item_like_c);
  item = new Item_compiled(jit_exec_ctx.get(), item_like_c);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 0);
}

TEST_F(JITItemCompiledTests, CompileItemFuncNe) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  const char *str_a = "Hello";
  const char *str_b = "Hello";
  const char *str_c = "YHello world! :D";

  Item *item_str_a = new Item_string(str_a, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_b = new Item_string(str_b, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_c = new Item_string(str_c, 16, &my_charset_utf8mb4_general_ci);
  Item *item_ne_a = new Item_func_ne(item_str_c, item_str_b);
  item_ne_a->fix_fields(thd(), &item_ne_a);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), item_ne_a);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);

  Item *item_ne_b = new Item_func_ne(item_str_a, item_str_b);
  item_ne_b->fix_fields(thd(), &item_ne_b);
  item = new Item_compiled(jit_exec_ctx.get(), item_ne_b);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  result = item->val_int();
  ASSERT_EQ(result, 0);
}

TEST_F(JITItemCompiledTests, CompileItemFuncNot) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  Item *item_int_a = new Item_int(10);
  Item *item_int_b = new Item_int(20);
  Item *item_le_c = new Item_func_le(item_int_a, item_int_b);
  Item *item_not_a = new Item_func_not(item_le_c);
  item_not_a->fix_fields(thd(), &item_not_a);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), item_not_a);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 0);
}

TEST_F(JITItemCompiledTests, CompileItemCondAnd) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  const char *str_a = "Hello";
  const char *str_b = "Hello";
  const char *str_c = "Hello world! :D";
  const char *str_d = "Hello world! :D";
  const char *str_e = "Hello world! :D ::";
  const char *str_f = "Hello world! :D :D";

  Item *item_str_a = new Item_string(str_a, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_b = new Item_string(str_b, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_c = new Item_string(str_c, 15, &my_charset_utf8mb4_general_ci);
  Item *item_str_d = new Item_string(str_d, 15, &my_charset_utf8mb4_general_ci);
  Item *item_str_e = new Item_string(str_e, 18, &my_charset_utf8mb4_general_ci);
  Item *item_str_f = new Item_string(str_f, 18, &my_charset_utf8mb4_general_ci);

  Item *item_eq_a = new Item_func_eq(item_str_a, item_str_b);
  Item *item_eq_b = new Item_func_eq(item_str_c, item_str_d);
  Item *item_eq_c = new Item_func_eq(item_str_e, item_str_f);

  Item *item_and_a = new Item_cond_and(item_eq_b, item_eq_c);
  Item *item_and = new Item_cond_and(item_eq_a, item_and_a);
  item_and->fix_fields(thd(), &item_and);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), item_and);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 0);
}

TEST_F(JITItemCompiledTests, CompileItemCondOr) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  const char *str_a = "Hello";
  const char *str_b = "Helld";
  const char *str_c = "Hello world! :D";
  const char *str_d = "Hello world! ::";
  const char *str_e = "Hello world! :D :D";
  const char *str_f = "Hello world! :D :D";

  Item *item_str_a = new Item_string(str_a, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_b = new Item_string(str_b, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_c = new Item_string(str_c, 15, &my_charset_utf8mb4_general_ci);
  Item *item_str_d = new Item_string(str_d, 15, &my_charset_utf8mb4_general_ci);
  Item *item_str_e = new Item_string(str_e, 18, &my_charset_utf8mb4_general_ci);
  Item *item_str_f = new Item_string(str_f, 18, &my_charset_utf8mb4_general_ci);

  Item *item_eq_a = new Item_func_eq(item_str_a, item_str_b);
  Item *item_eq_b = new Item_func_eq(item_str_c, item_str_d);
  Item *item_eq_c = new Item_func_eq(item_str_e, item_str_f);

  Item *item_and_a = new Item_cond_or(item_eq_b, item_eq_c);
  Item *item_and = new Item_cond_or(item_eq_a, item_and_a);
  item_and->fix_fields(thd(), &item_and);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), item_and);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);
}

TEST_F(JITItemCompiledTests, CompileItemCondOrAnd) {
  auto jit_exec_ctx = jit::JITExecutionContext::new_exec_context();
  ASSERT_TRUE(jit_exec_ctx != nullptr);

  const char *str_a = "Hello";
  const char *str_b = "Hello";
  const char *str_c = "Hello world! :D";
  const char *str_d = "Hello world! ::";
  const char *str_e = "Hello world! :D :D";
  const char *str_f = "Hello world! :D :D";
  const char *str_g = "Yes, great one. I am the summoner.";
  const char *str_h = "Yes, great one. I am the summoner.";

  Item *item_str_a = new Item_string(str_a, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_b = new Item_string(str_b, 5, &my_charset_utf8mb4_general_ci);
  Item *item_str_c = new Item_string(str_c, 15, &my_charset_utf8mb4_general_ci);
  Item *item_str_d = new Item_string(str_d, 15, &my_charset_utf8mb4_general_ci);
  Item *item_str_e = new Item_string(str_e, 18, &my_charset_utf8mb4_general_ci);
  Item *item_str_f = new Item_string(str_f, 18, &my_charset_utf8mb4_general_ci);
  Item *item_str_g = new Item_string(str_g, 34, &my_charset_utf8mb4_general_ci);
  Item *item_str_h = new Item_string(str_h, 34, &my_charset_utf8mb4_general_ci);

  Item *item_eq_a = new Item_func_eq(item_str_a, item_str_b);
  Item *item_eq_b = new Item_func_eq(item_str_c, item_str_d);
  Item *item_eq_c = new Item_func_eq(item_str_e, item_str_f);
  Item *item_eq_d = new Item_func_eq(item_str_e, item_str_f);
  Item *item_eq_e = new Item_func_eq(item_str_g, item_str_h);

  Item *item_or_a = new Item_cond_or(item_eq_b, item_eq_c);
  Item_cond_and *item_and = new Item_cond_and(item_eq_a, item_or_a);
  item_and->add(item_eq_d);
  item_and->add(item_eq_e);

  Item *item_query = static_cast<Item *>(item_and);

  item_query->fix_fields(thd(), &item_query);
  Item_compiled *item = new Item_compiled(jit_exec_ctx.get(), item_query);

  item->codegen_item();
  item->print_ir();
  item->jit_compile(jit_exec_ctx.get());
  auto result = item->val_int();
  ASSERT_EQ(result, 1);
}