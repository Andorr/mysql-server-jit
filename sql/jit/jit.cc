#include <cassert>
#include <iostream>
#include <memory>
#include "optional"

#include "sql/current_thd.h"
#include "sql/iterators/composite_iterators.h"
#include "sql/join_optimizer/access_path.h"
#include "sql/sql_base.h"

#include "sql/item.h"

#include "llvm/Support/TargetSelect.h"

// COMPILABLE INCLUDES
#include "sql/jit/item_compile_children.h"
#include "sql/jit/item_compiled.h"
#include "sql/jit/jit.h"
#include "sql/jit/jit_common.h"
#include "sql/jit/jit_current_exec_ctx.h"
#include "sql/jit/jit_exec_ctx.h"
#include "sql/jit/jit_filter_iterator.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> jit::new_jit_exec_ctx() {
  return JITExecutionContext::new_exec_context();
}

Item_compiled *jit::create_item_compiled_from_item(
    THD *thd, JITExecutionContext *jit_exec_ctx, Item *item) {
  Item_compiled *compiled =
      new (thd->mem_root) Item_compiled(jit_exec_ctx, item);
  compiled->codegen_item();
  compiled->print_ir();
  compiled->jit_compile(jit_exec_ctx);
  return compiled;
}

void jit::initialize() {
  llvm::InitializeNativeTarget();
  llvm::InitializeNativeTargetAsmPrinter();
  llvm::InitializeNativeTargetAsmParser();

  jit::initialized = true;
}

bool jit::initialized = false;

void jit::compile_filter_iterator(AccessPath *path) {
  // COMPILABLE CAN COMPILE HERE?????
  // Check if can compile before setting condition
  // fprintf(stderr, "CHECKING IF SHOULD COMPILE: %d\n",
  // current_thd->variables.should_jit_compile);
  auto cur_thd = current_thd;
  if (cur_thd->variables.should_jit_compile) {
    // fprintf(stderr, "Should compile query\n");

    if (!jit::initialized) {
      jit::initialize();
    }

    if (current_exec_ctx == nullptr) {
      current_exec_ctx = jit::new_jit_exec_ctx().release();
    }

    auto condition = path->filter().condition;

    condition->can_compile();
    if (condition->can_compile_result) {
      // The entire where_cond item can be replaced by a Item_compiled
      // *replace where_cond with new item_compiled*
      Item_compiled *where_cond_compiled = jit::create_item_compiled_from_item(
          cur_thd, current_exec_ctx, condition);
      path->filter().condition = where_cond_compiled;
    } else {
      compile_children(cur_thd, current_exec_ctx, condition);
    }

    // delete jit_ctx;
  }
}