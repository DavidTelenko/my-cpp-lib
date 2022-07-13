#pragma once

#include <cassert>

#define CONCAT_IMPL(A, B) A##B
#define CONCAT(A, B) CONCAT_IMPL(A, B)

#if defined NDEBUG
#define consteval_assert(expr) void(0)
#else
#define consteval_assert(expr) \
    ((expr) ? void(0) : [] { assert(!#expr); }())
#endif