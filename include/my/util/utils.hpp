#pragma once
#ifndef MY_UTILITY_HPP
#define MY_UTILITY_HPP

#include <cmath>
#include <my/util/traits.hpp>

namespace my {

// annotations and helpers

template <my::pointer T>
using owner = T;

/**
 * @brief finalAction allows you to ensure something gets run at the end of a scope
 */
template <my::value F>
class FinalAction final {
   public:
    constexpr explicit FinalAction(F f) noexcept
        : f_(std::move(f)) {
    }

    FinalAction(FinalAction &&other) noexcept
        : f_(std::move(other.f_)),
          invoke_(std::exchange(other.invoke_, false)) {}

    FinalAction(const FinalAction &) = delete;
    FinalAction &operator=(const FinalAction &) = delete;
    FinalAction &operator=(FinalAction &&) = delete;

    ~FinalAction() noexcept {
        if (invoke_) f_();
    }

   private:
    F f_;
    bool invoke_{true};
};

/**
 * @brief make FinalAction convenience function
 *
 * @tparam F callable
 * @param f function to call at the end of scope
 * @return FinalAction<std::remove_cv_t<std::remove_reference_t<F>>>
 */
template <class F>
[[nodiscard]] constexpr auto
finally(F &&f) noexcept {
    return FinalAction<my::value_from_t<F>>(std::forward<F>(f));
}

// various miscellaneous stuff

template <class PairT, class WidthT>
PairT twoDimensionalIndex(size_t index, WidthT width) {
    return {index % width, index / width};
}

template <class PairT, class WidthT, class CoordGetter>
size_t oneDimensionalIndex(PairT point, WidthT width,
                           CoordGetter first, CoordGetter second) {
    return first(point) + (second(point) * width);  // first / i / x, second / j / y
}

template <class PairT, class WidthT, class CoordGetter>
size_t oneDimensionalIndex(PairT point, WidthT width) {
    return point.x + (point.y * width);  // first / i / x, second / j / y
}

// AssociativeContainer methods

/**
 * @brief Checks wether container contains key
 *
 * @tparam AssociativeContainer concept
 * @param container AssociativeContainer concept const&
 * @param key AssociativeContainer::key_type value to search for
 * @return true if container contains key
 */
template <class AssociativeContainer,
          class Key = typename AssociativeContainer::key_type,
          class Value = typename AssociativeContainer::mapped_type>
constexpr bool containsKey(const AssociativeContainer &container, Key &&key) {
    const auto it = container.find(std::forward<Key>(key));
    return (it != end(container));
}

// bit logic + reinterpret

template <my::arithmetic Number>
constexpr bool
isPowerOf2(Number n) {
    if constexpr (std::is_integral_v<Number>) {
        return n > 0 and n & (n - 1) == 0;
    } else {
        int32_t exponent;
        const Number mantissa = std::frexp(n, &exponent);
        return mantissa == Number{0.5};
    }
}

/**
 * @see https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
 *
 */
template <std::integral Number>
constexpr void set_bit(Number &number, Number bit) { number |= 1UL << bit; }

template <std::integral Number>
constexpr void clear_bit(Number &number, Number bit) { number &= ~(1UL << bit); }

template <std::integral Number>
constexpr void toggle_bit(Number &number, Number bit) { number ^= 1UL << bit; }

template <std::integral Number>
constexpr bool check_bit(Number number, Number bit) { return (number >> bit) & 1U; }

}  // namespace my

#ifndef DISABLE_MY_MACROS

#define CONCAT_IMPL(A, B) A##B
#define CONCAT(A, B) CONCAT_IMPL(A, B)

#ifdef USE_FINALLY_MACRO
#define finally(f) \
    const auto CONCAT(final_action_temporary_variable_at_line_, __LINE__) = my::finally([]() f)
#endif
#ifdef USE_SCOPE_EXIT_MACRO
#define ON_SCOPE_EXIT(f) \
    const auto CONCAT(scope_exit_temporary_variable_at_line_, __LINE__) = my::finally([]() f)
#endif

#endif

#endif  // MY_UTILITY_HPP