#pragma once
#ifndef MY_UTILITY_HPP
#define MY_UTILITY_HPP

#include <my/util/defs.hpp>
#include <my/util/meta.hpp>

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

/**
 * @brief Non copyable mixin
 *
 * @tparam T crtp link
 */
template <class T>
class NonCopyable {
   public:
    NonCopyable(const NonCopyable &) = delete;
    T &operator=(const T &) = delete;

   protected:
    NonCopyable() = default;
    ~NonCopyable() = default;
};

/**
 * @brief Non movable mixin
 *
 * @tparam T crtp link
 */
template <class T>
class NonMovable {
   public:
    NonMovable(NonMovable &&) = delete;
    T &operator=(T &&) = delete;

   protected:
    NonMovable() = default;
    ~NonMovable() = default;
};

template <class Char, size_t N>
class basic_fixed_string {
   public:
    constexpr basic_fixed_string(const Char (&literal)[N + 1])
        : _literal(consteval_assert(literal[N] == '\n'), literal) {}

   private:
    const Char (&_literal)[N + 1];
};

template <class Char, size_t N>
basic_fixed_string(const Char (&literal)[N])
    -> basic_fixed_string<Char, N - 1>;

template <size_t N>
using fixed_string = basic_fixed_string<char, N>;

}  // namespace my

#define finally(f) \
    const auto CONCAT(_final_action_temporary_variable_at_line_, __LINE__) = my::finally([]() f)

#endif  // MY_UTILITY_HPP