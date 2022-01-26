#ifndef JIT_EXEC_CTX_H
#define JIT_EXEC_CTX_H

#include "memory"

#include "llvm/ADT/StringRef.h"
#include "llvm/ExecutionEngine/JITSymbol.h"
#include "llvm/ExecutionEngine/Orc/CompileUtils.h"
#include "llvm/ExecutionEngine/Orc/Core.h"
#include "llvm/ExecutionEngine/Orc/ExecutionUtils.h"
#include "llvm/ExecutionEngine/Orc/IRCompileLayer.h"
#include "llvm/ExecutionEngine/Orc/JITTargetMachineBuilder.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/SectionMemoryManager.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/LLVMContext.h"

#include "jit_builder_ctx.h"

namespace jit {

using namespace llvm;
using namespace orc;

class JITExecutionContext {
 private:
  std::unique_ptr<ExecutionSession> exec_session;

  RTDyldObjectLinkingLayer object_layer;
  IRCompileLayer compile_layer;

  DataLayout dl;
  MangleAndInterner mangle;

  JITDylib &main_jd;

 public:
  JITExecutionContext(std::unique_ptr<ExecutionSession> exec_session,
                      JITTargetMachineBuilder jtmb, DataLayout dl)
      : exec_session(std::move(exec_session)),
        object_layer(*this->exec_session,
                     []() { return std::make_unique<SectionMemoryManager>(); }),
        compile_layer(*this->exec_session, object_layer,
                      std::make_unique<ConcurrentIRCompiler>(std::move(jtmb))),
        dl(std::move(dl)),
        mangle(*this->exec_session, this->dl),
        main_jd(this->exec_session->createBareJITDylib("<main>")) {
    main_jd.addGenerator(
        cantFail(DynamicLibrarySearchGenerator::GetForCurrentProcess(
            dl.getGlobalPrefix())));

    if (jtmb.getTargetTriple().isOSBinFormatCOFF()) {
      object_layer.setOverrideObjectFlagsWithResponsibilityFlags(true);
      object_layer.setAutoClaimResponsibilityForObjectSymbols(true);
    }
  }

  ~JITExecutionContext() {
    if (auto err = exec_session->endSession()) {
      exec_session->reportError(std::move(err));
    }
  }

  static std::unique_ptr<JITExecutionContext> new_exec_context();
  std::unique_ptr<JITBuilderContext> new_builder_context();

  // Helpers
  const DataLayout &get_data_layout() const { return dl; }

  JITDylib &get_main_jitdylib() { return main_jd; }

  Error add_module(ThreadSafeModule tsm);

  Expected<JITEvaluatedSymbol> lookup(StringRef name);
};
}  // namespace jit

#endif