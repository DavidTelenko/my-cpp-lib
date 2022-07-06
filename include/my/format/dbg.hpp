#pragma once

#include <my/format/format.hpp>

// #ifndef __GNUC__
// #error "This feature is only available for gcc compiler family"
// #endif

#ifdef NDEBUG
#define DBG(expr) (expr)
#else
#define DBG(expr) ({                                 \
    const auto _dbg_tmp = (expr);                    \
    my::printf(std::cerr, "[{}:{}] {} = {}\n",       \
               __FILE__, __LINE__, #expr, _dbg_tmp); \
    _dbg_tmp;                                        \
})
#endif

// auto a = 10 + DBG(2 * 3); // prints: [__FILE__:__LINE__] 2 * 3 = 6