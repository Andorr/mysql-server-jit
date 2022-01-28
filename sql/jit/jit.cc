#include <cassert>
#include <iostream>
#include "optional"

#include "jit.h"

#include "sql/item.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> new_jit_exec_ctx() {
  return JITExecutionContext::new_exec_context();
}
Item_compiled *jit::create_item_compiled_from_item(
    JITExecutionContext *jit_exec_ctx, Item *where_cond) {
  // TODO: Implement this
  return nullptr;
}