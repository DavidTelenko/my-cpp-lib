#pragma once
#ifndef MY_STATISTICS_HPP
#define MY_STATISTICS_HPP

#include <algorithm>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>
#include <functional>
#include <map>
#include <random>

namespace my {

/**
 * @brief Finds mean of numeric range
 *
 * @tparam It Forward iterator
 * @param b begin of range
 * @param e end of range
 * @return mean of range
 */
template <std::input_iterator It>
constexpr auto mean(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    value_t accum{};
    size_t i{};

    for (; b != e; b++, i++) accum += *b;

    return accum / static_cast<value_t>(i);
}

/**
 * @brief Calculates harmonic mean of the distribution
 * @see https://en.wikipedia.org/wiki/Harmonic_mean
 *
 * @tparam It Forward iterator
 * @param b begin of range
 * @param e end of range
 * @return harmonic mean of range
 */
template <std::input_iterator It>
constexpr auto harmonicMean(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    value_t result = 0.f;
    size_t count = 0;

    for (; b != e; ++b, ++count) {
        result += (1.f / *b);
    }

    return static_cast<value_t>(count / result);
}

/**
 * @brief Finds mode of range
 *
 * @tparam It Forward iterator
 * @param b begin of range
 * @param e end of range
 * @return auto typename std::iterator_traits<It>::value_type
 */
template <std::input_iterator It>
constexpr auto mode(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    if (b == e) return value_t{};

    std::map<value_t, size_t> amounts;

    for (; b != e; b++) amounts[*b]++;

    return std::max_element(
               amounts.begin(), amounts.end(),
               [](auto a, auto b) {
                   return a.second < b.second;
               })
        ->first;
}

/**
 * @brief Finds diff between max and min of numeric range
 *
 * @tparam It Forward iterator
 * @tparam Comp = std::less<typename std::iterator_traits<It>::value_type>
 * @param b begin of range
 * @param e end of range
 * @param comp comparator function for range
 * @return auto typename std::iterator_traits<It>::value_type
 */
template <std::input_iterator It,
          class Comp = std::less<typename std::iterator_traits<It>::value_type>>
constexpr auto diapason(It b, It e, Comp comp = Comp{}) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    if (b == e) return value_t{};
    return *std::max_element(b, e, comp) - *std::min_element(b, e, comp);
}

/**
 * @brief Finds median of numeric range, requires underlying container to be sorted
 *
 * @tparam It Forward iterator
 * @param b begin of range
 * @param e end of range
 * @return auto typename std::iterator_traits<It>::value_type
 */
template <std::input_iterator It>
constexpr auto median(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    if (b == e) return value_t{};

    float size = std::distance(b, e) / 2.f;
    auto lhs = *(b + std::ceil(size));
    auto rhs = *(b + std::floor(size));

    return (lhs + rhs) / static_cast<value_t>(2);
}

/**
 * @brief Finds left and right quartile of numeric range,
 * requires underlying container to be sorted
 *
 * @tparam It Forward iterator
 * @param b begin of range
 * @param e end of range
 * @return auto std::pair of It::value_type's
 */
template <std::input_iterator It>
constexpr auto quartile(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    if (b == e) return std::make_pair(value_t{}, value_t{});

    auto halfSize = (std::distance(b, e) / 2.f) + 1;  // +1 here for the iterators

    return std::make_pair(median(b, b + std::floor(halfSize)),
                          median(b + std::ceil(halfSize), e));
}

/**
 * @brief Finds variance of numeric range
 *
 * @tparam It Forward iterator
 * @param b begin of range
 * @param e end of range
 * @return It::value_type
 */
template <std::input_iterator It>
constexpr auto variance(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    if (b == e) return value_t{};

    const auto mean = my::mean(b, e);
    const auto dist = std::distance(b, e);

    value_t accum{};
    for (; b != e; b++) {
        const auto s = *b - mean;
        accum += s * s;
    }

    return accum / (dist - 1);
}

/**
 * @brief Finds standart deviation of numeric range
 *
 * @tparam It Input iterator
 * @param b begin of range
 * @param e end of range
 * @return It::value_type
 */
template <std::input_iterator It>
constexpr auto sdeviation(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    return std::sqrt(my::variance(b, e));
}

/**
 * @brief Finds skewness of numeric range
 *
 * @tparam It Input iterator
 * @tparam require_input_iter<It>
 * @param b begin of range
 * @param e end of range
 * @return It::value_type
 */
template <std::input_iterator It>
constexpr auto skewness(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    if (b == e) return value_t{};
    auto mean = my::mean(b, e);
    auto sdev = my::sdeviation(b, e);
    auto dist = std::distance(b, e) - 1;

    value_t accum{};
    for (; b != e; b++) {
        auto s = (*b - mean);
        accum += s * s * s;
    }

    return accum / (dist * (sdev * sdev * sdev));
}

/**
 * @brief Finds kurtosis of range
 *
 * @tparam It Input Iterator
 * @tparam require_input_iter<It>
 * @param b begin of range
 * @param e end of range
 * @return It::value_type
 */
template <std::input_iterator It>
constexpr auto kurtosis(It b, It e) {
    using value_t = typename std::iterator_traits<It>::value_type;
    static_assert(std::is_arithmetic_v<value_t>);

    if (b == e) return value_t{};
    auto mean = my::mean(b, e);
    auto dist = std::distance(b, e);

    value_t accum1{};
    value_t accum2{};
    for (; b != e; b++) {
        auto s = *b - mean;
        auto ss = s * s;
        accum1 += ss * ss;
        accum2 += ss;
    }

    return dist * accum1 / (accum2 * accum2);
}

}  // namespace my

#endif  // MY_STATISTICS_HPP