#ifndef ITEM_COMPILED_H
#define ITEM_COMPILED_H

#include <cassert>
#include <memory>

#include "jit_exec_ctx.h"

#include "../item.h"

typedef longlong (*int_func_ptr)();

class Item_compiled : public Item {
  std::unique_ptr<jit::JITBuilderContext> builder_ctx;
  Item *item;

  std::unique_ptr<uint64_t> compiled_func;

 public:
  Item_compiled(jit::JITExecutionContext *exec_ctx, Item *item) : Item() {
    this->builder_ctx = exec_ctx->new_builder_context();
    this->item = item;
    this->compiled_func = nullptr;
  }

  ~Item_compiled() {}

  enum Type type() const override { return COMPILED_ITEM; }

  void codegen_item();

  void jit_compile(jit::JITExecutionContext *exec_ctx);

  void print_ir();

  longlong val_int() override;

  virtual double val_real() override { return 0.0; }
  virtual String *val_str(String *str) override { return str; }

  virtual my_decimal *val_decimal(my_decimal *decimal_buffer) override {
    return decimal_buffer;
  }
  virtual bool get_date([[maybe_unused]] MYSQL_TIME *ltime,
                        [[maybe_unused]] my_time_flags_t fuzzydate) override {
    return false;
  }
  virtual bool get_time([[maybe_unused]] MYSQL_TIME *ltime) override {
    return false;
  }
};

#endif