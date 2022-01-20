#ifndef JIT_EXEC_CTX_H
#define JIT_EXEC_CTX_H

#include "memory"

#include "jit_builder_ctx.h"

namespace jit {

class JITExecutionContext {
 public:
  static std::unique_ptr<JITExecutionContext> new_exec_context();
  std::unique_ptr<JITBuilderContext> new_builder_context();
};
}  // namespace jit

#endif