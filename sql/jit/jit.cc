#include <iostream>
#include "optional"

#include "jit.h"

using namespace jit;

std::optional<std::unique_ptr<JITExecutionContext>> jit::new_jit_exec_ctx() {
  std::cout << "Hello world! :D" << std::endl;
  return std::nullopt;
}
std::optional<int> jit::compile_where_cond(
    std::unique_ptr<JITExecutionContext> *jit_exec_ctx, Item *where_cond) {
  return std::nullopt;
};