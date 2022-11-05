#pragma once

#include <chrono>
#include <my/format/format.hpp>
#include <my/util/defs.hpp>

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

/**
 * @brief simple timing macro for measuring and printing
 * time of execution of provided expression
 *
 */
#if defined(NDEBUG) && defined(MY_DONT_PRINT_TIMEIT_RESULTS)
#define timeit(times, expr) ({ (expr); })
#else
#define timeit(times, expr) ({                                            \
    using namespace std::chrono;                                          \
                                                                          \
    auto _times = (times);                                                \
    const auto amount = _times;                                           \
    assert(_times >= 1);                                                  \
                                                                          \
    const auto start = high_resolution_clock::now();                      \
    while (_times--) {                                                    \
        (expr);                                                           \
    }                                                                     \
                                                                          \
    const auto diff = high_resolution_clock::now() - start;               \
    const auto ns = duration_cast<nanoseconds>(diff).count();             \
    const auto ms = duration_cast<milliseconds>(diff).count();            \
                                                                          \
    my::printf(                                                           \
        std::cerr,                                                        \
        "[{}:{}] {}\n"                                                    \
        "  time spent:\n"                                                 \
        "    > total  : {} ns ({} ms)\n"                                  \
        "    > average: {} ns ({} ms)\n",                                 \
        __FILENAME__, __LINE__, #expr, ns, ms, ns / amount, ms / amount); \
})
#endif