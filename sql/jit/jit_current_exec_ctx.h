
#ifndef CURRENT_JIT_EXEC_CTX
#define CURRENT_JIT_EXEC_CTX

namespace jit {
class JITExecutionContext;
extern thread_local JITExecutionContext *current_exec_ctx;
}  // namespace jit

#endif  // CURRENT_JIT_EXEC_CTX
