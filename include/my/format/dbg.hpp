#pragma once

#include <my/format/format.hpp>

/**
 * @brief dirty debugging macro
 *
 * #Example:
 * auto a = 10 + dbg(2 * 3); // may print: [c:\dev\main.cpp:8] 2 * 3 = 6
 *
 */
#if defined(NDEBUG) || !defined(__GNUC__)
#define dbg(expr) (expr)
#else
#define dbg(expr) ([]() {                            \
    const auto _dbg_tmp = (expr);                    \
    my::printf(std::cerr, "[{}:{}] {} = {}\n",       \
               __FILE__, __LINE__, #expr, _dbg_tmp); \
    return _dbg_tmp;                                 \
}())
#endif