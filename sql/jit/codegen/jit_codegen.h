#ifndef JIT_CODEGEN_H
#define JIT_CODEGEN_H

#include "llvm/IR/Value.h"
#include "sql/jit/jit_builder_ctx.h"

class Item_int;

namespace jit {
llvm::Value *codegen_item_int(Item_int *item, jit::JITBuilderContext *context);

}

#endif
