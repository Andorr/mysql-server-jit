#include "jit_common.h"
#include "sql/jit/item_compiled.h"

#include "sql/item.h"
#include "sql/item_cmpfunc.h"
#include "sql/jit/jit.h"
#include "sql/jit/jit_exec_ctx.h"

#include "sql/jit/item_compile_children.h"

// Function to do stuff, aka recursive greier

void compile_children(THD *thd, jit::JITExecutionContext *jit_execution_context,
                      Item *item) {
  if (Item_int *int_item = dynamic_cast<Item_int *>(item)) {
    // Item int doesnt have children and should have been compiled by its parent
    // item
    (void)int_item;
    return;
  } else if (Item_func *item_func_eq = dynamic_cast<Item_func *>(item)) {
    auto items = item_func_eq->arguments();

    if (items[0] != nullptr && items[0]->can_compile_result) {
      Item_compiled *left_child_compiled = jit::create_item_compiled_from_item(
          jit_execution_context, item_func_eq->arguments()[0]);
      item_func_eq->set_arg(thd, 0, left_child_compiled);
    } else {
      compile_children(thd, jit_execution_context,
                       item_func_eq->arguments()[0]);
    }
    if (items[1] != nullptr && items[1]->can_compile_result) {
      Item_compiled *right_child_compiled = jit::create_item_compiled_from_item(
          jit_execution_context, item_func_eq->arguments()[1]);
      item_func_eq->set_arg(thd, 1, right_child_compiled);
    } else {
      compile_children(thd, jit_execution_context,
                       item_func_eq->arguments()[1]);
    }
  } else if (Item_field *item_field = dynamic_cast<Item_field *>(item)) {
    // Item field doesnt have children and should have been compiled by its
    // parent item
    (void)item_field;
    return;
  }
}
