#include "memory"

#include "jit_exec_ctx.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> JITExecutionContext::new_exec_context() {
  return nullptr;
}