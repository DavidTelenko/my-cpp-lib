#pragma once

#include <my/format/repr.hpp>
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
_printf(std::basic_ostream<Ch, Tr>& os, const Ch* format) {
    os << format;
}

template <class Ch, class Tr,
          my::representable<std::basic_ostream<Ch, Tr>> Arg,
          my::representable<std::basic_ostream<Ch, Tr>>... Args>
constexpr void
_printf(std::basic_ostream<Ch, Tr>& os,
        const Ch* format, Arg&& arg, Args&&... args) {
    for (; *format != '\0'; ++format) {
        if (*format == '{') {
            if (*(format + 1) != '}') {
                os << '{';
                continue;
            }
            my::represent(os, arg);
            return detail::_printf(os, (format + 2),
                                   std::forward<Args>(args)...);
        }
        os << *format;
    }
}

}  // namespace detail

/**
 * @brief Prints formatted output into provided std::ostream.
 * Replaces each next '{}' occurance with next argument
 *
 * @tparam Args
 * @param os std::ostream where to print data
 * @param format format c_str where '{}' is a replace anchor
 * @param args any representable types
 */
template <class Ch, class Tr,
          my::representable<std::basic_ostream<Ch, Tr>>... Args>
constexpr void
printf(std::basic_ostream<Ch, Tr>& os,
       const Ch* format, Args&&... args) {
    detail::_printf(os, format, std::forward<Args>(args)...);
}

/**
 * @brief Prints formatted output into std::cout / std::wcout.
 * Replaces each next '{}' occurance with next argument
 *
 * @param format format c_str where '{}' is a replace anchor
 * @param args any representable types
 */
template <class Ch, my::representable<std::basic_ostream<Ch>>... Args>
constexpr void
printf(const Ch* format, Args&&... args) {
    if constexpr (std::same_as<Ch, wchar_t>) {
        detail::_printf(std::wcout, format, std::forward<Args>(args)...);
    } else {
        detail::_printf(std::cout, format, std::forward<Args>(args)...);
    }
}

/**
 * @brief Creates new std::string object with formatted ouput
 *
 * @tparam Args
 * @param format format c_str where '{}' is a replace anchor
 * @param args any types with std::ostream& operator<< implemented
 *
 * @return std::string new object with printed ouput
 */
template <class Ch,
          my::representable<std::basic_ostream<Ch>>... Args>
[[nodiscard]] inline auto
format(const Ch* format, Args&&... args) {
    std::basic_stringstream<Ch> ss;
    detail::_printf(ss, format, std::forward<Args>(args)...);
    return ss.str();
}

}  // namespace my