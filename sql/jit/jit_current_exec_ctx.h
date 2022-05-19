
#ifndef CURRENT_JIT_EXEC_CTX
#define CURRENT_JIT_EXEC_CTX

namespace jit {
class JITExecutionContext;
extern thread_local JITExecutionContext *current_exec_ctx;

extern thread_local int instruction_count_fd;
}  // namespace jit

#endif  // CURRENT_JIT_EXEC_CTX
