#pragma once

#include <my/format/format.hpp>

// #ifndef __GNUC__
// #error "This feature is only available for gcc compiler family"
// #endif

/**
 * @brief dirty debugging macro
 *
 * #Example:
 * auto a = 10 + DBG(2 * 3); // prints: [__FILE__:__LINE__] 2 * 3 = 6
 *
 */
#ifdef NDEBUG
#define dbg(expr) (expr)
#else
#define dbg(expr) ({                                 \
    const auto _dbg_tmp = (expr);                    \
    my::printf(std::cerr, "[{}:{}] {} = {}\n",       \
               __FILE__, __LINE__, #expr, _dbg_tmp); \
    _dbg_tmp;                                        \
})
#endif