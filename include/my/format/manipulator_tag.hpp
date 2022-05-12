#pragma once
#ifndef MY_MANIPULATOR_TAG_HPP
#define MY_MANIPULATOR_TAG_HPP

namespace my {

struct ManipulatorTag {};

namespace detail {
template <class T>
using get_manipulator_tag = typename T::manipulator_tag;
}

template <class T>
using has_manipulator_tag = my::applicable<detail::get_manipulator_tag, T>;

template <class T>
inline constexpr bool has_manipulator_tag_v = has_manipulator_tag<T>::value;

// /**
//  * @brief Wrapper for function call directly
//  * into std::ostream omiting any printer logic,
//  * common use case with my::alt::printf,
//  * when you want to apply manipulator instead of printing.
//  *
//  * e.g my::alt::printf("{}, {}, {}") << f << s << my::manip(std::boolalpha) << true;
//  *
//  * @note Please note that manipulators applied to inner ostream will not be erased after call.
//  *
//  * @tparam Function
//  */
// template <class Function>
// struct manip {
//     using manipulator_tag = ManipulatorTag;

//     inline constexpr manip(Function manipulator) noexcept
//         : m_(std::move(manipulator)) {
//     }

//     template <class F, class Printer>
//     friend const auto& operator<<(const print_t<Printer>& p,
//                                   const manip<F>& obj) {
//         p->put(obj.m_);
//         return p;
//     }

//     mutable my::make_member_function_t<Function> m_;
// };

}  // namespace my

#endif  // MY_MANIPULATOR_TAG_HPP
