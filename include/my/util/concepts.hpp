#pragma once
#ifndef MY_TRAITS_HPP
#define MY_TRAITS_HPP

#include <iostream>
#include <iterator>
#include <ranges>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace my {

// tuple-like

template <class T, std::size_t N>
concept has_tuple_element =
    requires(T t) {
    typename std::tuple_element_t<N, std::remove_const_t<T>>;
    { get<N>(t) } -> std::convertible_to<const std::tuple_element_t<N, T>&>;
};

// clang-format off

template <class T>
concept tuple_like =
    not std::is_reference_v<T> and requires(T t) {
        typename std::tuple_size<T>::type;
        requires std::derived_from<
            std::tuple_size<T>,
            std::integral_constant<std::size_t, std::tuple_size_v<T>>>;
    } and []<std::size_t... N>(std::index_sequence<N...>) {
        return (has_tuple_element<T, N> and ...);
    }(std::make_index_sequence<std::tuple_size_v<T>>());

// clang-format on

// pair-like

template <class T>
concept pair_like =
    not std::is_reference_v<T> and requires(T t) {
    typename std::tuple_size<T>::type;
    requires std::derived_from<
        std::tuple_size<T>,
        std::integral_constant<std::size_t, 2>>;
    requires has_tuple_element<T, 0>;
    requires has_tuple_element<T, 1>;
};

// TODO follow https://en.cppreference.com/w/cpp/named_req/AssociativeContainer
template <class T>
concept associative_container = requires(T val) {
    typename T::key_type;
    typename T::mapped_type;
    typename T::value_type;
};

template <class T>
using range_size_t = decltype(std::ranges::size(std::declval<T>()));

template <class T>
concept reservable_range = std::ranges::sized_range<T> and requires(
    std::remove_cvref_t<T>& rng, range_size_t<T> size) {
    rng.reserve(size);
};

template <class T>
concept erasable_range = std::ranges::range<T> and requires(
    std::remove_cvref_t<T>& rng, std::ranges::iterator_t<T> iter) {
    { rng.erase(iter) } -> std::same_as<std::ranges::iterator_t<T>>;
};

template <class T>
concept push_back_callable_range = std::ranges::range<T> and requires(
    std::remove_cvref_t<T>& rng, std::ranges::range_value_t<T> val) {
    rng.push_back(val);
};

template <class T>
concept value = requires {
    not(std::is_reference_v<T> or
        std::is_volatile_v<T> or
        std::is_const_v<T>);
};

template <class T, class Ostream = std::ostream,
          class R = std::remove_reference_t<T>,
          class O = std::remove_reference_t<Ostream>>
concept printable = requires(O& os, const R& obj) {
    { os << obj } -> std::same_as<O&>;
};

template <class T>
concept arithmetic = std::is_arithmetic_v<T>;

template <class T>
concept pointer = std::is_pointer_v<T>;

/// functional

template <class F>
struct function_traits;

template <class R, class... Args>
struct function_traits<R (*)(Args...)>
    : public function_traits<R(Args...)> {};

template <class R, class... Args>
struct function_traits<R(Args...)> {
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    requires(N < arity) struct argument {
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...)>
    : public function_traits<R(C&, Args...)> {};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...) const>
    : public function_traits<R(C&, Args...)> {};

template <class C, class R>
struct function_traits<R(C::*)>
    : public function_traits<R(C&)> {};

template <class F>
struct function_traits {
   private:
    using call_type = function_traits<decltype(&F::type::operator())>;

   public:
    using return_type = typename call_type::return_type;

    static constexpr std::size_t arity = call_type::arity - 1;

    template <std::size_t N>
    requires(N < arity) struct argument {
        using type = typename call_type::template argument<N + 1>::type;
    };
};

/// transformers

template <class T>
using value_from = std::remove_cv<std::remove_reference_t<T>>;

template <class T>
using value_from_t = typename value_from<T>::type;

template <class F>
using make_member_function =
    std::conditional<std::is_function_v<F>, std::add_pointer_t<F>, F>;

template <class F>
using make_member_function_t = typename make_member_function<F>::type;

}  // namespace my

#endif  // MY_TRAITS_HPP
