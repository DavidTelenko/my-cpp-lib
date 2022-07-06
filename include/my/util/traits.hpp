#pragma once
#ifndef MY_TRAITS_HPP
#define MY_TRAITS_HPP

#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <utility>

namespace my {

// applicable

namespace detail {

template <template <class...> class Trait, class = void, class... Ts>
struct applicable : std::false_type {};

template <template <class...> class Trait, class... Ts>
struct applicable<Trait, std::void_t<Trait<Ts...>>, Ts...> : std::true_type {};

}  // namespace detail

template <template <class...> class Trait, class... Ts>
using applicable = detail::applicable<Trait, void, Ts...>;

template <template <class...> class Trait, class... Ts>
constexpr bool applicable_v = applicable<Trait, Ts...>::value;

// is iterator

template <class I, class = void>
struct is_iterator : std::false_type {};
template <>
struct is_iterator<void*, void> : std::false_type {};
template <>
struct is_iterator<void const*, void> : std::false_type {};
template <>
struct is_iterator<void volatile*, void> : std::false_type {};
template <>
struct is_iterator<void const volatile*, void> : std::false_type {};
template <class I>
struct is_iterator<I, std::void_t<typename std::iterator_traits<I>::value_type>>
    : std::true_type {};
template <typename I>
constexpr bool is_iterator_v = is_iterator<I>::value;

// is tuple

template <typename>
struct is_tuple : std::false_type {};
template <typename... T>
struct is_tuple<std::tuple<T...>> : std::true_type {};

template <typename T>
constexpr bool is_tuple_v = is_tuple<T>::value;

// is pair

template <typename>
struct is_pair : std::false_type {};
template <typename T, typename U>
struct is_pair<std::pair<T, U>> : std::true_type {};

template <typename T>
constexpr bool is_pair_v = is_pair<T>::value;

// is associative container

namespace detail {
template <class T>
using get_key_type = typename T::key_type;

template <class T>
using get_mapped_type = typename T::mapped_type;
};  // namespace detail

template <class T>
using is_associative_container = std::conjunction<
    applicable<detail::get_key_type, T>,
    applicable<detail::get_mapped_type, T>>;

template <typename T>
constexpr bool is_associative_container_v = is_associative_container<T>::value;

template <class T>
concept associative_container = my::is_associative_container_v<T>;

// has print operator

namespace detail {

template <class T, class Ostream>
using call_print_operator = decltype(std::declval<Ostream&>()
                                     << std::declval<const T&>());
}  // namespace detail

template <class T, class Ostream = std::ostream>
using has_print_operator = applicable<detail::call_print_operator, T, Ostream>;

template <class T, class Ostream = std::ostream>
constexpr bool has_print_operator_v = has_print_operator<T, Ostream>::value;

// has reserve

namespace detail {

template <class T, class SizeT>
using call_reserve =
    decltype(std::declval<T>().reserve(std::declval<SizeT>()));
}

template <class T, class SizeT = std::size_t>
using has_reserve = my::applicable<detail::call_reserve, T, SizeT>;

template <class T, class SizeT>
constexpr bool has_reserve_v = has_reserve<T, SizeT>::value;

template <class T, class SizeT = typename T::size_type>
concept reservable = requires(std::remove_reference_t<T>& obj, SizeT size) {
    obj.reserve(size);
};

// is comparable

namespace detail {

template <class T, class U>
using call_less_than = decltype(std::declval<T>() < std::declval<U>());

template <class T, class U>
using call_equals = decltype(std::declval<T>() == std::declval<U>());

}  // namespace detail

template <class T, class U>
using is_equality_comparable = std::conjunction<
    applicable<detail::call_equals, T, U>,
    applicable<detail::call_equals, U, T>>;

template <class T, class U>
using is_comparable = std::conjunction<
    applicable<detail::call_less_than, T, U>,
    applicable<detail::call_less_than, U, T>>;

template <class T, class U>
constexpr bool is_comparable_v = is_comparable<T, U>::value;

template <class T, class U>
constexpr bool is_equality_comparable_v = is_equality_comparable<T, U>::value;

// is incrementable / decrementable

namespace detail {

template <class T>
using call_increment = decltype(++std::declval<std::remove_reference_t<T>&>());

template <class T>
using call_decrement = decltype(--std::declval<std::remove_reference_t<T>&>());

}  // namespace detail

template <class T>
using is_incrementable = my::applicable<detail::call_increment, T>;

template <class T>
using is_decrementable = my::applicable<detail::call_decrement, T>;

template <class T>
constexpr bool is_incrementable_v = is_incrementable<T>::value;

template <class T>
constexpr bool is_decrementable_v = is_decrementable<T>::value;

// is iterable

namespace detail {

using std::begin;
using std::end;

template <class T>
using call_begin = decltype(begin(std::declval<T>()));
template <class T>
using call_end = decltype(end(std::declval<T>()));

}  // namespace detail

template <class T>
using is_iterable = std::conjunction<
    applicable<detail::call_begin, T>,
    applicable<detail::call_end, T>>;

template <class T>
constexpr bool is_iterable_v = is_iterable<T>::value;

template <class T>
concept iterable = my::is_iterable_v<T>;

// is value

// not (reference or volatile or const)
template <class T>
using is_value =
    std::negation<std::disjunction<std::is_reference<T>,
                                   std::is_volatile<T>,
                                   std::is_const<T>>>;

template <class T>
constexpr bool is_value_v = is_value<T>::value;

template <class T>
concept value = my::is_value_v<T>;

// make value

template <class T>
using value_from = std::remove_cv<std::remove_reference_t<T>>;

template <class T>
using value_from_t = typename value_from<T>::type;

// functional

template <class F>
struct function_traits;

template <class R, class... Args>
struct function_traits<R (*)(Args...)> : public function_traits<R(Args...)> {};

template <class R, class... Args>
struct function_traits<R(Args...)> {
    using return_type = R;

    static constexpr std::size_t arity = sizeof...(Args);

    template <std::size_t N>
    struct argument {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename std::tuple_element<N, std::tuple<Args...>>::type;
    };
};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...)> : public function_traits<R(C&, Args...)> {};

template <class C, class R, class... Args>
struct function_traits<R (C::*)(Args...) const> : public function_traits<R(C&, Args...)> {};

template <class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C&)> {};

template <class F>
struct function_traits {
   private:
    using call_type = function_traits<decltype(&F::type::operator())>;

   public:
    using return_type = typename call_type::return_type;

    static constexpr std::size_t arity = call_type::arity - 1;

    template <std::size_t N>
    struct argument {
        static_assert(N < arity, "error: invalid parameter index.");
        using type = typename call_type::template argument<N + 1>::type;
    };
};

template <class F>
using make_member_function =
    std::conditional<std::is_function_v<F>, std::add_pointer_t<F>, F>;

template <class F>
using make_member_function_t = typename make_member_function<F>::type;

// callers and signature checks, details for functional and algorithms
namespace detail {

using std::begin;
using std::end;

template <my::iterable Iterable>
using iterator_t = decltype(begin(std::declval<Iterable>()));
}  // namespace detail

template <my::iterable T>
using iterator_t = detail::iterator_t<T>;

template <my::iterable Iterable>
using value_t =
    typename std::iterator_traits<iterator_t<Iterable>>::value_type;

namespace detail {

template <my::iterable Iterable>
using call_push_back =
    decltype(std::declval<Iterable>().push_back(std::declval<value_t<Iterable>>()));

template <my::iterable Iterable>
using call_insert =
    decltype(std::declval<Iterable>().insert(std::declval<value_t<Iterable>>()));

template <my::iterable Iterable>
using call_square_brackets =
    decltype(std::declval<Iterable>()[0] = std::declval<value_t<Iterable>>());

template <my::iterable Iterable>
using call_push_front =
    decltype(std::declval<Iterable>().push_front(std::declval<value_t<Iterable>>()));

}  // namespace detail

// concepts

template <class T, class Ostream = std::ostream,
          class R = std::remove_reference_t<T>,
          class O = std::remove_reference_t<Ostream>>
concept printable = requires(O& os, const R& obj) {
    { os << obj } -> std::same_as<O&>;
};

template <class T>
concept iterator_concept = my::is_iterator_v<T>;

template <class T>
concept arithmetic = std::is_arithmetic_v<T>;

template <class T>
concept pointer = std::is_pointer_v<T>;

}  // namespace my

#endif  // MY_TRAITS_HPP
