#pragma once
#ifndef MY_COLOR_FORMAT_HPP
#define MY_COLOR_FORMAT_HPP

#include <my/format/color_presets.hpp>
#include <my/format/format.hpp>
#include <my/util/utils.hpp>
//
#include <iostream>

namespace my {

/**
 * @brief canonical color structure with 3 uint8_t as r, g, b values
 *
 */
struct Color {
    using enum color;

    constexpr Color()
        : r(0), g(0), b(0) {}

    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : r(r), g(g), b(b) {}

    constexpr Color(uint32_t hex)
        : Color(((hex >> 16) & 0xFF),
                ((hex >> 8) & 0xFF),
                (hex & 0xFF)) {}

    constexpr Color(my::color c)
        : Color(static_cast<uint32_t>(c)) {}

    constexpr uint32_t toHex() {
        return (static_cast<uint32_t>(r) << 16) |
               (static_cast<uint32_t>(g) << 8) | b;
    }

    constexpr bool operator==(uint32_t hex) { return toHex() == hex; }
    constexpr bool operator!=(uint32_t hex) { return toHex() != hex; }

    uint8_t r, g, b;
};

/**
 * @brief Sets foreground into os
 *
 * @param os output stream
 * @param foreground color structure, can be implicitly created from uint32_t
 * @return output stream reference
 */
template <class Ch, class Tr>
inline auto& setfg(std::basic_ostream<Ch, Tr>& os, Color foreground) {
    os << "\033[38;2;";  // FIXME widen me
    os << +foreground.r;
    os.put(os.widen(';'));
    os << +foreground.g;
    os.put(os.widen(';'));
    os << +foreground.b;
    os.put(os.widen('m'));
    return os;
}

/**
 * @brief Sets background into os
 *
 * @param os output stream
 * @param background color structure, can be implicitly created from uint32_t
 * @return output stream reference
 */
template <class Ch, class Tr>
inline auto& setbg(std::basic_ostream<Ch, Tr>& os, Color background) {
    os << "\033[48;2;";  // FIXME widen me
    os << +background.r;
    os.put(os.widen(';'));
    os << +background.g;
    os.put(os.widen(';'));
    os << +background.b;
    os.put(os.widen('m'));
    return os;
}

/**
 * @brief Sets both foreground and background into os
 *
 * @param os output stream
 * @param foreground color structure, can be implicitly created from uint32_t
 * @param background color structure, can be implicitly created from uint32_t
 * @return output stream reference
 */
template <class Ch, class Tr>
inline auto& setcol(std::basic_ostream<Ch, Tr>& os,
                    Color foreground,
                    Color background) {
    setfg(os, std::move(foreground));
    setbg(os, std::move(background));
    return os;
}

/**
 * @brief Manipulator to reset color of stream
 *
 * @param os output stream reference
 * @return reference to os
 */
template <class C, class T>
inline auto& resetcol(std::basic_ostream<C, T>& os) {
    os << "\033[0m";  // FIXME widen me
    return os;
}

/**
 * @brief Manipulator proxy of setfg color
 *
 */
struct setForeground {
    inline constexpr setForeground(Color foreground) noexcept
        : fg_(std::move(foreground)) {}

    template <class Ch, class Tr>
    friend inline auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                                   const setForeground& fg) {
        return setfg(os, fg.fg_);
    }

   private:
    const Color fg_;
};

/**
 * @brief Manipulator proxy of setbg color
 *
 */
struct setBackground {
    inline constexpr setBackground(Color background) noexcept
        : bg_(std::move(background)) {}

    template <class Ch, class Tr>
    friend inline auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                                   const setBackground& bg) {
        return setbg(os, bg.bg_);
    }

   private:
    const Color bg_;
};

/**
 * @brief Manipulator proxy of setbg and setfg color
 *
 */
struct setColor {
    inline constexpr setColor(Color foreground, Color background) noexcept
        : fg_(std::move(foreground)), bg_(std::move(background)) {}

    template <class Ch, class Tr>
    friend inline auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                                   const setColor& col) {
        return setcol(os, col.fg_, col.bg_);
    }

   private:
    const Color fg_;
    const Color bg_;
};

/**
 * @brief Reset color manipulator proxy
 *
 */
struct resetcol_t {
    template <class Ch, class Tr>
    friend inline auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                                   const resetcol_t& bg) {
        return resetcol(os);
    }
};

/**
 * @brief Convenience function to create setForeground manipulator,
 * @note can be called "make_set_foreground"
 *
 * @param foreground color structure, can be implicitly created from uint32_t
 * @return setForeground manipulator
 */
[[nodiscard]] inline constexpr auto fg(Color foreground) noexcept {
    return setForeground(std::move(foreground));
}

/**
 * @brief Convenience function to create setBackground manipulator,
 * @note can be called "make_set_background"
 *
 * @param background color structure, can be implicitly created from uint32_t
 * @return setBackground manipulator
 */
[[nodiscard]] inline constexpr auto bg(Color background) noexcept {
    return setBackground(std::move(background));
}

/**
 * @brief Convenience function to create setColor manipulator,
 * @note can be called "make_set_background"
 *
 * @param foreground color structure, can be implicitly created from uint32_t
 * @param background color structure, can be implicitly created from uint32_t
 * @return setColor manipulator
 */
[[nodiscard]] inline constexpr auto col(Color foreground,
                                        Color background) noexcept {
    return setColor(std::move(foreground), std::move(background));
}

/**
 * @brief Manipulator to reset color of stream
 *
 * @param os output stream reference
 * @return reference to os
 */
inline constexpr resetcol_t resetcol() noexcept { return resetcol_t{}; }

/**
 * @brief Prints colored formatted output
 *
 * @param os output stream reference
 * @param foreground color structure, can be implicitly created from uint32_t
 * @param background color structure, can be implicitly created from uint32_t
 * @param format format string
 * @param args any joinable or printable value
 */
template <class Ch, class Tr, my::joinable<Ch, Tr>... Args>
inline void printf(std::basic_ostream<Ch, Tr>& os,
                   Color foreground, Color background,
                   const Ch* format, Args&&... args) {
    setcol(os, foreground, background);
    printf(os, format, args...);
    os << resetcol;
}

/**
 * @brief Prints colored formatted output
 *
 * @param os output stream reference
 * @param foreground color structure, can be implicitly created from uint32_t
 * @param format format string
 * @param args any joinable or printable value
 */
template <my::joinable<char, std::char_traits<char>>... Args>
inline void printf(std::ostream& os, Color foreground,
                   const char* format, Args&&... args) {
    setfg(os, foreground);
    printf(os, format, args...);
    os << resetcol;
}

/**
 * @brief Prints colored formatted output into std::cout
 *
 * @param foreground color structure, can be implicitly created from uint32_t
 * @param background color structure, can be implicitly created from uint32_t
 * @param format format string
 * @param args any joinable or printable value
 */
template <my::joinable<char, std::char_traits<char>>... Args>
inline void printf(Color foreground, Color background,
                   const char* format, Args&&... args) {
    printf(std::cout, foreground, background, format, args...);
}

/**
 * @brief Prints colored formatted output into std::cout
 *
 * @param foreground color structure, can be implicitly created from uint32_t
 * @param format format string
 * @param args any joinable or printable value
 */
template <my::joinable<char, std::char_traits<char>>... Args>
inline void printf(Color foreground,
                   const char* format, Args&&... args) {
    printf(std::cout, foreground, format, args...);
}

}  // namespace my

#endif  // MY_COLOR_FORMAT_HPP