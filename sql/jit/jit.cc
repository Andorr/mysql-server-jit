#include <cassert>
#include <iostream>
#include "optional"

#include "jit.h"

#include "sql/item.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> jit::new_jit_exec_ctx() {
  return JITExecutionContext::new_exec_context();
}
std::optional<int> jit::compile_where_cond(
    [[maybe_unused]] std::unique_ptr<JITExecutionContext> *jit_exec_ctx,
    [[maybe_unused]] Item *where_cond) {
  return std::nullopt;
};