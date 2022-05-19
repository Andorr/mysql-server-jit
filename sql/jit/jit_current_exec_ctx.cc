#include "sql/jit/jit_current_exec_ctx.h"

using namespace jit;

thread_local JITExecutionContext *jit::current_exec_ctx = nullptr;

thread_local int jit::instruction_count_fd = -1;