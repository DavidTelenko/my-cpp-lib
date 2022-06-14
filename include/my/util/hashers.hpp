#pragma once

#include <tuple>
#include <utility>

namespace my {

/**
 * @brief Combines two hashes in a safe way and writes result to first parameter
 *
 * @param lhs hash1
 * @param rhs hash2
 * @return new lhs value
 */
size_t hashCombine(size_t &lhs, size_t rhs) {
    lhs ^= rhs + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

/**
 * @brief Combines two hashes in a safe way and writes result to first parameter
 *
 * @param lhs hash1
 * @param rhs value to hash (uses std::hash<T>)
 * @return new lhs value
 */
template <class T>
size_t hashCombine(size_t &lhs,
                   const T &rhs) requires(not std::same_as<T, size_t>) {
    lhs ^= std::hash<T>{}(rhs) + 0x9e3779b9 + (lhs << 6) + (lhs >> 2);
    return lhs;
}

struct TupleHash {
    template <class... Ts>
    constexpr size_t operator()(const std::tuple<Ts...> &tpl) const {
        return [&]<size_t... I>(size_t seed, std::index_sequence<I...>) {
            (hashCombine(seed, std::get<I>(tpl)), ...);
            return seed;
        }
        (0, std::make_index_sequence<sizeof...(Ts)>{});
    }
};

constexpr TupleHash hashTuple{};

struct ArgsHash {
    template <class... Ts>
    constexpr size_t operator()(Ts &&...args) const {
        size_t seed = 0;
        (hashCombine(seed, std::forward<Ts>(args)), ...);
        return seed;
    }
};

constexpr ArgsHash hashArgs{};

}  // namespace my
