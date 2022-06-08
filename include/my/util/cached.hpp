#pragma once

#include <my/util/hashers.hpp>
#include <unordered_map>
#include <unordered_set>

namespace my {

/**
 * @brief Pure function decorator that will store preferred amount of
 * arguments-result pair and return cached value if called with same arguments
 *
 */
template <class Sig>
class cached;

/**
 * @brief Pure function decorator that will store preferred amount of
 * arguments-result pair and return cached value if called with same arguments.
 * Note side effects are not presumed.
 *
 */
template <class Res, class... Args>
class cached<Res(Args...)> {
   public:
    template <class Function>
    constexpr explicit cached(Function func, size_t cacheLimit = 1000)
        : _func(std::move(func)), _limit(cacheLimit) {
    }

    Res operator()(Args... args) const {
        const auto hash = hashArgs(args...);

        if constexpr (not std::same_as<Res, void>) {
            static std::unordered_map<size_t, Res> cache;

            const auto it = cache.find(hash);

            if (it != cache.end()) return it->second;
            if (!_limit) return _func(args...);

            --_limit;

            const auto mem = _func(args...);
            cache[hash] = mem;

            return mem;
        } else {
            static std::unordered_set<size_t> cache;

            const auto it = cache.find(hash);

            if (it != cache.end()) return;
            if (!_limit) return _func(args...);

            --_limit;

            _func(args...);
            cache.insert(hash);
        }
    }

   private:
    std::function<Res(Args...)> _func;
    mutable size_t _limit;
};

}  // namespace my