#pragma once
#ifndef MY_DELIMITERS_HPP
#define MY_DELIMITERS_HPP

#include <my/util/traits.hpp>

namespace my {

template <class T>
struct delimiters {
};

template <my::iterable T>
struct delimiters<T> {
    inline constexpr static const char* open = "[";
    inline constexpr static const char* delim = ", ";
    inline constexpr static const char* close = "]";
};

template <my::iterator_concept T>
struct delimiters<T> {
    inline constexpr static const char* open = "[";
    inline constexpr static const char* delim = ", ";
    inline constexpr static const char* close = "]";
};

template <class... Ts>
struct delimiters<std::tuple<Ts...>> {
    inline constexpr static const char* open = "(";
    inline constexpr static const char* delim = ", ";
    inline constexpr static const char* close = ")";
};

template <class... Ts>
struct delimiters<std::pair<Ts...>> {
    inline constexpr static const char* open = "(";
    inline constexpr static const char* delim = ", ";
    inline constexpr static const char* close = ")";
};

}  // namespace my

#endif  // MY_DELIMITERS_HPP