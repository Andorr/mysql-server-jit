#include <cassert>
#include <iostream>
#include <memory>
#include "optional"

#include "sql/current_thd.h"
#include "sql/iterators/composite_iterators.h"
#include "sql/sql_base.h"

#include "sql/item.h"

#include "llvm/Support/TargetSelect.h"

// COMPILABLE INCLUDES
#include "sql/jit/item_compile_children.h"
#include "sql/jit/item_compiled.h"
#include "sql/jit/jit.h"
#include "sql/jit/jit_common.h"
#include "sql/jit/jit_exec_ctx.h"
#include "sql/jit/jit_filter_iterator.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> jit::new_jit_exec_ctx() {
  return JITExecutionContext::new_exec_context();
}

Item_compiled *jit::create_item_compiled_from_item(
    JITExecutionContext *jit_exec_ctx, Item *item) {
  Item_compiled *compiled = new Item_compiled(jit_exec_ctx, item);
  compiled->codegen_item();
  // compiled->print_ir();
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

void jit::compile_filter_iterator(FilterIterator *iterator) {
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

    auto *jit_ctx = jit::new_jit_exec_ctx().release();

    iterator->m_condition->can_compile();
    if (iterator->m_condition->can_compile_result) {
      // The entire where_cond item can be replaced by a Item_compiled
      // *replace where_cond with new item_compiled*
      Item_compiled *where_cond_compiled =
          jit::create_item_compiled_from_item(jit_ctx, iterator->m_condition);
      iterator->m_condition = where_cond_compiled;
    } else {
      compile_children(cur_thd, jit_ctx, iterator->m_condition);
    }
  }
}