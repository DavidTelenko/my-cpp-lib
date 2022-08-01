#pragma once

#include <cassert>
#include <cstring>

#define CONCAT_IMPL(A, B) A##B
#define CONCAT(A, B) CONCAT_IMPL(A, B)

#if defined NDEBUG
#define consteval_assert(expr) void(0)
#else
#define consteval_assert(expr) \
    ((expr) ? void(0) : [] { assert(!#expr); }())
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 \
                                              : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 \
                                             : __FILE__)
#endif