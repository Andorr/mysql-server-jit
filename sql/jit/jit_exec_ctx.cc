#include "iostream"
#include "memory"
#include "optional"

#include "llvm/ADT/StringRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/PassManager.h"
#include "llvm/IR/Value.h"

#include "jit_common.h"
#include "jit_exec_ctx.h"

#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/IPO/StripDeadPrototypes.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/ADCE.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Utils.h"

using namespace jit;

std::unique_ptr<JITExecutionContext> JITExecutionContext::new_exec_context() {
  auto epc = SelfExecutorProcessControl::Create();
  if (!epc) {
    // debug_print("error: %s", "SelfExecutorProcessControl");
    return nullptr;
  }

  auto exec_session = std::make_unique<ExecutionSession>(std::move(*epc));

  JITTargetMachineBuilder jtmb(
      exec_session->getExecutorProcessControl().getTargetTriple());

  auto dl = jtmb.getDefaultDataLayoutForTarget();
  if (!dl) {
    // debug_print("error: %s", "SelfExecutorProcessControl");
    return nullptr;
  }

  return std::make_unique<JITExecutionContext>(std::move(exec_session),
                                               std::move(jtmb), std::move(*dl));
}

std::unique_ptr<JITBuilderContext> JITExecutionContext::new_builder_context() {
  auto builder_context = std::make_unique<JITBuilderContext>();

  builder_context->context = std::make_unique<LLVMContext>();

  builder_context->builder =
      std::make_unique<llvm::IRBuilder<>>(*builder_context->context);

  builder_context->func_module = std::make_unique<llvm::Module>(
      "item_compiled", *builder_context->context);
  builder_context->func_module->setDataLayout(this->get_data_layout());

  builder_context->pass_manager = std::make_unique<llvm::FunctionPassManager>();

  return builder_context;
}

Error JITExecutionContext::add_module(ThreadSafeModule tsm) {
  auto rt = this->main_jd.getDefaultResourceTracker();
  return compile_layer.add(rt, std::move(tsm));
}

Expected<JITEvaluatedSymbol> JITExecutionContext::lookup(StringRef name) {
  return exec_session->lookup({&main_jd}, mangle(name.str()));
}

llvm::Expected<llvm::orc::ThreadSafeModule> JITExecutionContext::optimizeModule(
    llvm::orc::ThreadSafeModule tsm,
    const llvm::orc::MaterializationResponsibility &r) {
  return optimizeModuleWithoutMR(std::move(tsm));
}

llvm::Expected<llvm::orc::ThreadSafeModule>
JITExecutionContext::optimizeModuleWithoutMR(llvm::orc::ThreadSafeModule tsm) {
  tsm.withModuleDo([](llvm::Module &m) {
    // Create a function pass manager
    auto pm = std::make_unique<llvm::legacy::PassManager>();

    // Add some optimizations
    pm->add(llvm::createInstructionCombiningPass());
    pm->add(llvm::createReassociatePass());
    pm->add(llvm::createNewGVNPass());
    pm->add(llvm::createCFGSimplificationPass());
    auto inlinePass = llvm::createFunctionInliningPass();

    pm->add(inlinePass);
    pm->add(llvm::createAggressiveDCEPass());
    pm->add(llvm::createStripDeadPrototypesPass());
    pm->run(m);

    auto fpm = std::make_unique<llvm::legacy::FunctionPassManager>(&m);

    // Add some optimizations
    fpm->add(llvm::createInstructionCombiningPass());
    fpm->add(llvm::createReassociatePass());
    fpm->add(llvm::createNewGVNPass());
    fpm->add(llvm::createCFGSimplificationPass());
    // fpm->add(llvm::createAggressiveDCEPass());
    // fpm->add(llvm::createStripDeadPrototypesPass());

    fpm->doInitialization();

    for (auto &F : m) {
      fpm->run(F);
    }
  });

  return std::move(tsm);
}