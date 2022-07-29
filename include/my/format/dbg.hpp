#pragma once

#include <string.h>

#include <chrono>
#include <my/format/format.hpp>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 \
                                              : __FILE__)
#else
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 \
                                             : __FILE__)
#endif

/**
 * @brief dirty debugging macro
 *
 * #Example:
 * auto a = 10 + dbg(2 * 3); // may print: [c:\dev\main.cpp:8] 2 * 3 = 6
 *
 */
#if defined(NDEBUG)
#define dbg(expr) (expr)
#else
#define dbg(expr) ([]() {                                \
    const auto _dbg_tmp = (expr);                        \
    my::printf(std::cerr, "[{}:{}] {} = {}\n",           \
               __FILENAME__, __LINE__, #expr, _dbg_tmp); \
    return _dbg_tmp;                                     \
}())
#endif

#if defined(NDEBUG)
#define timeit(...)
#else
#define timeit(expr, times) ({                                                   \
    using namespace std::chrono;                                                 \
                                                                                 \
    auto _times = (times);                                                       \
    const auto amount = _times;                                                  \
    assert(_times >= 1);                                                         \
                                                                                 \
    const auto start = high_resolution_clock::now();                             \
    while (--_times) {                                                           \
        (expr);                                                                  \
    }                                                                            \
                                                                                 \
    const auto diff = high_resolution_clock::now() - start;                      \
    const auto ns = duration_cast<nanoseconds>(diff).count();                    \
    const auto ms = duration_cast<milliseconds>(diff).count();                   \
                                                                                 \
    my::printf(std::cerr,                                                        \
               "[{}:{}] {}\n"                                                    \
               "    time spent:\n"                                               \
               "    > total  : {} ns ({} ms)\n"                                  \
               "    > average: {} ns ({} ms)\n",                                 \
               __FILENAME__, __LINE__, #expr, ns, ms, ns / amount, ms / amount); \
})
#endif