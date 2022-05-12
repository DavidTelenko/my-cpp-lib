#pragma once
#ifndef MY_SYMBOLS_HPP
#define MY_SYMBOLS_HPP

namespace my {

enum class Style {
    Ascii,
    Basic,
    Light,
    Heavy,
    Double,
    Curvy,
    LightDashed,
    HeavyDashed,
};

inline constexpr const char* const styles[][11] = {
    // 0    1    2    3    4    5    6    7    8    9    A
    // [Ascii] =
    {"-", "|", "+", "+", "+", "+", "+", "+", "+", "+", "+"},
    // [Basic] =
    {"—", "|", "+", "+", "+", "+", "+", "+", "+", "+", "+"},
    // [Light] =
    {"─", "│", "┌", "┬", "┐", "├", "┼", "┤", "└", "┴", "┘"},
    // [Heavy] =
    {"━", "┃", "┏", "┳", "┓", "┣", "╋", "┫", "┗", "┻", "┛"},
    // [Double] =
    {"═", "║", "╔", "╦", "╗", "╠", "╬", "╣", "╚", "╩", "╝"},
    // [Curvy] =
    {"─", "│", "╭", "┬", "╮", "├", "┼", "┤", "╰", "┴", "╯"},
    // [LightDashed] =
    {"╌", "╎", "┌", "┬", "┐", "├", "┼", "┤", "└", "┴", "┘"},
    // [HeavyDashed] =
    {"╍", "╏", "┏", "┳", "┓", "┣", "╋", "┫", "┗", "┻", "┛"},
};

}  // namespace my

#endif  // MY_SYMBOLS_HPP