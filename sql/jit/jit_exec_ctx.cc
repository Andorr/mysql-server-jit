#include "memory"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"

#include "jit_exec_ctx.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> JITExecutionContext::new_exec_context() {
  return nullptr;
}

std::unique_ptr<JITBuilderContext> JITExecutionContext::new_builder_context() {
  return nullptr;
}

Error JITExecutionContext::add_module(ThreadSafeModule tsm) {
  auto rt = this->main_jd.getDefaultResourceTracker();
  return compile_layer.add(rt, std::move(tsm));
}

Expected<JITEvaluatedSymbol> JITExecutionContext::lookup(StringRef name) {
  return exec_session->lookup({&main_jd}, mangle(name.str()));
}