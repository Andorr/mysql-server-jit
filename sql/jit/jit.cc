#include <iostream>
#include "optional"

#include "jit.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> jit::new_jit_exec_ctx() {
  return JITExecutionContext::new_exec_context();
}
std::optional<int> jit::compile_where_cond(
    std::unique_ptr<JITExecutionContext> *jit_exec_ctx, Item *where_cond) {
  return std::nullopt;
};