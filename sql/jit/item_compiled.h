#ifndef ITEM_COMPILED_H
#define ITEM_COMPILED_H

#include <cassert>
#include <memory>

#include "sql/jit/jit_exec_ctx.h"

#include "../item.h"

typedef longlong (*int_func_ptr)();
std::string gen_random(const int len);

class Item_compiled : public Item {
  std::unique_ptr<jit::JITBuilderContext> builder_ctx;
  Item *item;

  std::string name;

  std::unique_ptr<uint64_t> compiled_func;

    // To avoid a lot of repetitive writing.
  using steady_clock = std::chrono::steady_clock;
  template <class T>
  using duration = std::chrono::duration<T>;

  steady_clock::time_point now() const {
#if defined(__linux__)
    // Work around very slow libstdc++ implementations of std::chrono
    // (those compiled with _GLIBCXX_USE_CLOCK_GETTIME_SYSCALL).
    timespec tp;
    clock_gettime(CLOCK_MONOTONIC, &tp);
    return steady_clock::time_point(
        steady_clock::duration(std::chrono::seconds(tp.tv_sec) +
                               std::chrono::nanoseconds(tp.tv_nsec)));
#else
    return steady_clock::now();
#endif
  }


  // COMPILABLE COMPILE TIME TIMING 
  std::chrono::steady_clock::time_point::duration compile_time{0};

 public:
  Item_compiled(jit::JITExecutionContext *exec_ctx, Item *item) : Item() {
    this->builder_ctx = exec_ctx->new_builder_context();
    this->item = item;
    this->compiled_func = nullptr;
    this->name = gen_random(12);
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

  void print(const THD *thd, String *str,
             enum_query_type query_type) const override;
};

#endif