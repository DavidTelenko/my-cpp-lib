#pragma once

namespace my {

inline namespace fmt {

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

constexpr const char* const styles[][11] = {
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

}  // namespace fmt

}  // namespace my