#include "sql/jit/jit_current_exec_ctx.h"

using namespace jit;

thread_local JITExecutionContext *jit::current_exec_ctx = nullptr;
