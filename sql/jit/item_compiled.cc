#include <unistd.h>
#include <ctime>
#include <iostream>
#include <memory>

#include "jit_common.h"
#include "sql/jit/codegen/jit_codegen.h"
#include "sql/jit/item_compiled.h"
#include "string"

#include "sql/current_thd.h"
#include "sql/sql_class.h"

#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/Argument.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"

void Item_compiled::codegen_item() {
  // COMPILABLEC CODEGEN TIME TIMETAKING
  steady_clock::time_point start;
  steady_clock::time_point end;

  // Create function
  if (current_thd->variables.should_time_compile_time) {
    start = now();
  }
  // We always return an 64-bit integer
  auto return_type = llvm::Type::getInt64Ty(*builder_ctx->context);
  llvm::FunctionType *functionType =
      llvm::FunctionType::get(return_type, false);

  llvm::Function *function =
      llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                             this->name.c_str(), *builder_ctx->func_module);
  builder_ctx->main_function = function;

  // Set entry point
  llvm::BasicBlock *entry =
      llvm::BasicBlock::Create(*builder_ctx->context, "entry", function);
  builder_ctx->builder->SetInsertPoint(entry);

  llvm::Value *value = jit::codegen_item(this->item, builder_ctx.get());
  builder_ctx->builder->CreateRet(value);
  if (current_thd->variables.should_time_compile_time) {
    end = now();
    codegen_time += end - start;
  }
}

void Item_compiled::jit_compile(jit::JITExecutionContext *exec_ctx,
                                bool optimize) {
  // COMPILABLEC COMPILE TIME TIMETAKING
  steady_clock::time_point start;
  steady_clock::time_point end;

  llvm::ExitOnError exit_on_err;
  auto TSM = llvm::orc::ThreadSafeModule(std::move(builder_ctx->func_module),
                                         std::move(builder_ctx->context));

  if (optimize) {
    // auto optTSM =
    //     exit_on_err(exec_ctx->optimizeModuleWithoutMR(std::move(TSM)));
    // optTSM.withModuleDo(
    //     [](llvm::Module &m) { m.print(llvm::errs(), nullptr); });
    TSM = exit_on_err(exec_ctx->optimizeModuleWithoutMR(std::move(TSM)));
  }

  // TODO: Replace exit_on_err with better error-handling
  if (auto err = exec_ctx->add_module(std::move(TSM))) {
    // debug_print("error: was not able to add module");
    return;
  }

  // TODO: Replace exit_on_err with better error-handling
  // COMPILABLE COMPILE TIME ACTUAL TIMETAKING
  if (current_thd->variables.should_time_compile_time) {
    start = now();
  }
  if (auto expr_symbol = exec_ctx->lookup(this->name.c_str())) {
    this->compiled_func =
        std::make_unique<uint64_t>((uint64_t)(*expr_symbol).getAddress());
  } else {
    // debug_print("error: was not able to lookup __main");
  }
  if (current_thd->variables.should_time_compile_time) {
    end = now();
    compile_time += end - start;
  }
}

longlong Item_compiled::val_int() {
  assert(this->compiled_func != nullptr);

  int_func_ptr func = reinterpret_cast<int_func_ptr>(*this->compiled_func);
  return func();
}

void Item_compiled::print_ir() {
  builder_ctx->main_function->print(llvm::errs());
  fprintf(stderr, "\n");
}

void Item_compiled::print(const THD *thd, String *str,
                          enum_query_type query_type) const {
  double codegen_time_ms = duration<double>(codegen_time).count() * 1e3;
  double compile_time_ms = duration<double>(compile_time).count() * 1e3;

  const char *format_str = "Item_compiled[%s](codegen=%.3f compile=%.3f) ";

  int str_size = std::snprintf(nullptr, 0, format_str, this->name.c_str(),
                               codegen_time_ms, compile_time_ms);
  auto size = static_cast<size_t>(str_size);

  auto buf = std::make_unique<char[]>(size);

  std::snprintf(buf.get(), size, format_str, this->name.c_str(),
                codegen_time_ms, compile_time_ms);

  // str->reserve(sizeof("Item_compiled") - 1);
  // str->append(STRING_WITH_LEN("Item_compiled"));

  str->reserve(str_size - 1);
  str->append(buf.get(), size - 1);

  this->item->print(thd, str, query_type);
}

bool Item_compiled::is_expensive() { return this->item->is_expensive(); }

std::string gen_random(const int len) {
  static const char alphanum[] =
      "0123456789"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "abcdefghijklmnopqrstuvwxyz";
  std::string tmp_s;
  tmp_s.reserve(len);

  for (int i = 0; i < len; ++i) {
    tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
  }

  return tmp_s;
}
