#ifndef JIT_COMPILE_CHILDREN
#define JIT_COMPILE_CHILDREN

#include "sql/item.h"
#include "sql/jit/jit_exec_ctx.h"
#include "sql/sql_class.h"

void compile_children(THD *thd, jit::JITExecutionContext *jit_execution_context,
                      Item *item);

#endif