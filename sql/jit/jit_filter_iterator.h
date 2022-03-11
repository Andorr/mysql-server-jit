#ifndef H_JIT_FILTER_ITERATOR
#define H_JIT_FILTER_ITERATOR

#include <memory>

#include "sql/jit/jit_exec_ctx.h"
#include "sql/join_optimizer/access_path.h"

class FilterIterator;

namespace jit {
void compile_filter_iterator(AccessPath *path);
}

#endif