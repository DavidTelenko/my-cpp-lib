#pragma once

#include <my/util/hashers.hpp>
//
#include <functional>
#include <optional>
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

template <class Sig>
class once;

/**
 * @brief Calls provided function only once.
 * Uses std::optional to store the result of the function,
 * or bool if the function returns void
 *
 */
template <class Res, class... Args>
class once<Res(Args...)> {
   public:
    template <class Function>
    constexpr explicit once(Function func)
        : _func(std::move(func)) {
    }

    Res operator()(Args... args) const {
        if constexpr (not std::same_as<Res, void>) {
            static std::optional<Res> value;
            if (not value) value = _func(args...);
            return *value;
        } else {
            static bool called = false;
            if (not called) {
                _func(args...);
                called = true;
            }
        }
    }

   private:
    std::function<Res(Args...)> _func;
};

}  // namespace my