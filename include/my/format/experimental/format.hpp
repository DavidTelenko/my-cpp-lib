#pragma once
#ifndef MY_FORMAT_HPP
#define MY_FORMAT_HPP

#include <my/format/join.hpp>
//
#include <array>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace my {

namespace detail {

template <class Ch, class Tr>
[[maybe_unused]] constexpr void
printf(std::basic_ostream<Ch, Tr>& os, const Ch* format) {
    os << format;
}

template <class Ch, class Tr,
          my::joinable<Ch, Tr> Arg,
          my::joinable<Ch, Tr>... Args>
constexpr void
printf(std::basic_ostream<Ch, Tr>& os,
       const Ch* format, Arg&& arg, Args&&... args) {
    for (; *format != '\0'; ++format) {
        if (*format == '{') {
            if (*(format + 1) != '}') {
                os << '{';
                continue;
            }
            os << join<Ch, Tr>(arg);
            return detail::printf(os, (format + 2),
                                  std::forward<Args>(args)...);
        }
        os << *format;
    }
}

}  // namespace detail

/**
 * @brief Prints formatted output into std::cout.
 * Replaces each next '{}' occurance with next argument
 *
 * @tparam Args
 * @param format format c_str where '{}' is a replace anchor
 * @param args any types with std::ostream& operator<< implemented
 */
template <my::joinable<char, std::char_traits<char>>... Args>
constexpr void
printf(const char* format, Args&&... args) {
    detail::printf(std::cout, format, std::forward<Args>(args)...);
}

/**
 * @brief Prints formatted output into std::wcout.
 * Replaces each next '{}' occurance with next argument
 *
 * @tparam Args
 * @param format format wc_str where '{}' is a replace anchor
 * @param args any types with std::wostream& operator<< implemented
 */
template <my::joinable<wchar_t, std::char_traits<wchar_t>>... Args>
constexpr void
wprintf(const wchar_t* format, Args&&... args) {
    detail::printf(std::wcout, format, std::forward<Args>(args)...);
}

/**
 * @brief Prints formatted output into provided std::ostream.
 * Replaces each next '{}' occurance with next argument
 *
 * @tparam Args
 * @param os std::ostream where to print data
 * @param format format c_str where '{}' is a replace anchor
 * @param args any types with std::ostream& operator<< implemented
 */
template <class Ch, class Tr, my::joinable<Ch, Tr>... Args>
constexpr void
printf(std::basic_ostream<Ch, Tr>& os,
       const Ch* format, Args&&... args) {
    detail::printf(os, format, std::forward<Args>(args)...);
}

/**
 * @brief Creates new std::string object with formated ouput
 *
 * @tparam Args
 * @param format format c_str where '{}' is a replace anchor
 * @param args any types with std::ostream& operator<< implemented
 *
 * @return std::string new object with printed ouput
 */
template <class Ch, my::joinable<Ch, std::char_traits<Ch>>... Args>
inline std::basic_string<Ch, std::char_traits<Ch>>
format(const Ch* format, Args&&... args) {
    std::basic_stringstream<Ch, std::char_traits<Ch>> ss;
    detail::printf(ss, format, std::forward<Args>(args)...);
    return ss.str();
}

}  // namespace my

#endif  // MY_FORMAT_HPP