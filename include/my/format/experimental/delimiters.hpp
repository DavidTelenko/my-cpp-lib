#pragma once
#ifndef MY_DELIMITERS_HPP
#define MY_DELIMITERS_HPP

#include <my/util/concepts.hpp>

namespace my::experimental {

template <class T>
struct delimiters {
};

template <my::iterable T>
struct delimiters<T> {
    constexpr static const char* open = "[";
    constexpr static const char* delim = ", ";
    constexpr static const char* close = "]";
};

template <my::iterator_concept T>
struct delimiters<T> {
    constexpr static const char* open = "[";
    constexpr static const char* delim = ", ";
    constexpr static const char* close = "]";
};

template <class... Ts>
struct delimiters<std::tuple<Ts...>> {
    constexpr static const char* open = "(";
    constexpr static const char* delim = ", ";
    constexpr static const char* close = ")";
};

template <class... Ts>
struct delimiters<std::pair<Ts...>> {
    constexpr static const char* open = "(";
    constexpr static const char* delim = ", ";
    constexpr static const char* close = ")";
};

}  // namespace my

#endif  // MY_DELIMITERS_HPP