#include <cassert>
#include <iostream>
#include <memory>
#include "optional"

#include "jit.h"

#include "sql/item.h"

#include "llvm/Support/TargetSelect.h"

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