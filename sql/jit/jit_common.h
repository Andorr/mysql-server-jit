#ifndef JIT_COMMON_H
#define JIT_COMMON_H

#ifdef DEBUG
#include <stdio.h>
#define debug_print(fmt, ...)                                           \
  do {                                                                  \
    if (DEBUG)                                                          \
      fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, \
              __VA_ARGS__);                                             \
  } while (0)
#else
#define debug_print(fmt, ...) \
  do {                        \
  } while (0)
#endif

#endif