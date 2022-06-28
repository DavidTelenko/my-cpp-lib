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