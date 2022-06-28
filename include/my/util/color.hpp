#pragma once
#ifndef MY_COLOR_HPP
#define MY_COLOR_HPP

#include <cstdint>
#include <iostream>

namespace my {

/**
 * @brief canonical color structure with 3 uint8_t as r, g, b values
 *
 */
struct Color {
    enum class Preset : uint32_t {
        AliceBlue = 0xF0F8FF,             // rgb(240, 248, 255)
        AntiqueWhite = 0xFAEBD7,          // rgb(250, 235, 215)
        Aqua = 0x00FFFF,                  // rgb(000, 255, 255)
        Aquamarine = 0x7FFFD4,            // rgb(127, 255, 212)
        Azure = 0xF0FFFF,                 // rgb(240, 255, 255)
        Beige = 0xF5F5DC,                 // rgb(245, 245, 220)
        Bisque = 0xFFE4C4,                // rgb(255, 228, 196)
        Black = 0x000000,                 // rgb(000, 000, 000)
        BlanchedAlmond = 0xFFEBCD,        // rgb(255, 235, 205)
        Blue = 0x0000FF,                  // rgb(000, 000, 255)
        BlueViolet = 0x8A2BE2,            // rgb(138, 043, 226)
        Brown = 0xA52A2A,                 // rgb(165, 042, 042)
        BurlyWood = 0xDEB887,             // rgb(222, 184, 135)
        CadetBlue = 0x5F9EA0,             // rgb(095, 158, 160)
        Chartreuse = 0x7FFF00,            // rgb(127, 255, 000)
        Chocolate = 0xD2691E,             // rgb(210, 105, 030)
        Coral = 0xFF7F50,                 // rgb(255, 127, 080)
        CornflowerBlue = 0x6495ED,        // rgb(100, 149, 237)
        Cornsilk = 0xFFF8DC,              // rgb(255, 248, 220)
        Crimson = 0xDC143C,               // rgb(220, 020, 060)
        Cyan = 0x00FFFF,                  // rgb(000, 255, 255)
        DarkBlue = 0x00008B,              // rgb(000, 000, 139)
        DarkCyan = 0x008B8B,              // rgb(000, 139, 139)
        DarkGoldenRod = 0xB8860B,         // rgb(184, 134, 011)
        DarkGray = 0xA9A9A9,              // rgb(169, 169, 169)
        DarkGreen = 0x006400,             // rgb(000, 100, 000)
        DarkKhaki = 0xBDB76B,             // rgb(189, 183, 107)
        DarkMagenta = 0x8B008B,           // rgb(139, 000, 139)
        DarkOliveGreen = 0x556B2F,        // rgb(085, 107, 047)
        DarkOrange = 0xFF8C00,            // rgb(255, 140, 000)
        DarkOrchid = 0x9932CC,            // rgb(153, 050, 204)
        DarkRed = 0x8B0000,               // rgb(139, 000, 000)
        DarkSalmon = 0xE9967A,            // rgb(233, 150, 122)
        DarkSeaGreen = 0x8FBC8F,          // rgb(143, 188, 143)
        DarkSlateBlue = 0x483D8B,         // rgb(072, 061, 139)
        DarkSlateGray = 0x2F4F4F,         // rgb(047, 079, 079)
        DarkTurquoise = 0x00CED1,         // rgb(000, 206, 209)
        DarkViolet = 0x9400D3,            // rgb(148, 000, 211)
        DeepPink = 0xFF1493,              // rgb(255, 020, 147)
        DeepSkyBlue = 0x00BFFF,           // rgb(000, 191, 255)
        DimGray = 0x696969,               // rgb(105, 105, 105)
        DodgerBlue = 0x1E90FF,            // rgb(030, 144, 255)
        FireBrick = 0xB22222,             // rgb(178, 034, 034)
        FloralWhite = 0xFFFAF0,           // rgb(255, 250, 240)
        ForestGreen = 0x228B22,           // rgb(034, 139, 034)
        Fuchsia = 0xFF00FF,               // rgb(255, 000, 255)
        Gainsboro = 0xDCDCDC,             // rgb(220, 220, 220)
        GhostWhite = 0xF8F8FF,            // rgb(248, 248, 255)
        Gold = 0xFFD700,                  // rgb(255, 215, 000)
        GoldenRod = 0xDAA520,             // rgb(218, 165, 032)
        Gray = 0x808080,                  // rgb(128, 128, 128)
        Green = 0x008000,                 // rgb(000, 128, 000)
        GreenYellow = 0xADFF2F,           // rgb(173, 255, 047)
        HoneyDew = 0xF0FFF0,              // rgb(240, 255, 240)
        HotPink = 0xFF69B4,               // rgb(255, 105, 180)
        IndianRed = 0xCD5C5C,             // rgb(205, 092, 092)
        Indigo = 0x4B0082,                // rgb(075, 000, 130)
        Ivory = 0xFFFFF0,                 // rgb(255, 255, 240)
        Khaki = 0xF0E68C,                 // rgb(240, 230, 140)
        Lavender = 0xE6E6FA,              // rgb(230, 230, 250)
        LavenderBlush = 0xFFF0F5,         // rgb(255, 240, 245)
        LawnGreen = 0x7CFC00,             // rgb(124, 252, 000)
        LemonChiffon = 0xFFFACD,          // rgb(255, 250, 205)
        LightBlue = 0xADD8E6,             // rgb(173, 216, 230)
        LightCoral = 0xF08080,            // rgb(240, 128, 128)
        LightCyan = 0xE0FFFF,             // rgb(224, 255, 255)
        LightGoldenRodYellow = 0xFAFAD2,  // rgb(250, 250, 210)
        LightGray = 0xD3D3D3,             // rgb(211, 211, 211)
        LightGreen = 0x90EE90,            // rgb(144, 238, 144)
        LightPink = 0xFFB6C1,             // rgb(255, 182, 193)
        LightSalmon = 0xFFA07A,           // rgb(255, 160, 122)
        LightSeaGreen = 0x20B2AA,         // rgb(032, 178, 170)
        LightSkyBlue = 0x87CEFA,          // rgb(135, 206, 250)
        LightSlateGray = 0x778899,        // rgb(119, 136, 153)
        LightSteelBlue = 0xB0C4DE,        // rgb(176, 196, 222)
        LightYellow = 0xFFFFE0,           // rgb(255, 255, 224)
        Lime = 0x00FF00,                  // rgb(000, 255, 000)
        LimeGreen = 0x32CD32,             // rgb(050, 205, 050)
        Linen = 0xFAF0E6,                 // rgb(250, 240, 230)
        Magenta = 0xFF00FF,               // rgb(255, 000, 255)
        Maroon = 0x800000,                // rgb(128, 000, 000)
        MediumAquamarine = 0x66CDAA,      // rgb(102, 205, 170)
        MediumBlue = 0x0000CD,            // rgb(000, 000, 205)
        MediumOrchid = 0xBA55D3,          // rgb(186, 085, 211)
        MediumPurple = 0x9370DB,          // rgb(147, 112, 219)
        MediumSeaGreen = 0x3CB371,        // rgb(060, 179, 113)
        MediumSlateBlue = 0x7B68EE,       // rgb(123, 104, 238)
        MediumSpringGreen = 0x00FA9A,     // rgb(000, 250, 154)
        MediumTurquoise = 0x48D1CC,       // rgb(072, 209, 204)
        MediumVioletRed = 0xC71585,       // rgb(199, 021, 133)
        MidnightBlue = 0x191970,          // rgb(025, 025, 112)
        MintCream = 0xF5FFFA,             // rgb(245, 255, 250)
        MistyRose = 0xFFE4E1,             // rgb(255, 228, 225)
        Moccasin = 0xFFE4B5,              // rgb(255, 228, 181)
        NavajoWhite = 0xFFDEAD,           // rgb(255, 222, 173)
        Navy = 0x000080,                  // rgb(000, 000, 128)
        OldLace = 0xFDF5E6,               // rgb(253, 245, 230)
        Olive = 0x808000,                 // rgb(128, 128, 000)
        OliveDrab = 0x6B8E23,             // rgb(107, 142, 035)
        Orange = 0xFFA500,                // rgb(255, 165, 000)
        OrangeRed = 0xFF4500,             // rgb(255, 069, 000)
        Orchid = 0xDA70D6,                // rgb(218, 112, 214)
        PaleGoldenRod = 0xEEE8AA,         // rgb(238, 232, 170)
        PaleGreen = 0x98FB98,             // rgb(152, 251, 152)
        PaleTurquoise = 0xAFEEEE,         // rgb(175, 238, 238)
        PaleVioletRed = 0xDB7093,         // rgb(219, 112, 147)
        PapayaWhip = 0xFFEFD5,            // rgb(255, 239, 213)
        PeachPuff = 0xFFDAB9,             // rgb(255, 218, 185)
        Peru = 0xCD853F,                  // rgb(205, 133, 063)
        Pink = 0xFFC0CB,                  // rgb(255, 192, 203)
        Plum = 0xDDA0DD,                  // rgb(221, 160, 221)
        PowderBlue = 0xB0E0E6,            // rgb(176, 224, 230)
        Purple = 0x800080,                // rgb(128, 000, 128)
        RebeccaPurple = 0x663399,         // rgb(102, 051, 153)
        Red = 0xFF0000,                   // rgb(255, 000, 000)
        RosyBrown = 0xBC8F8F,             // rgb(188, 143, 143)
        RoyalBlue = 0x4169E1,             // rgb(065, 105, 225)
        SaddleBrown = 0x8B4513,           // rgb(139, 069, 019)
        Salmon = 0xFA8072,                // rgb(250, 128, 114)
        SandyBrown = 0xF4A460,            // rgb(244, 164, 096)
        SeaGreen = 0x2E8B57,              // rgb(046, 139, 087)
        SeaShell = 0xFFF5EE,              // rgb(255, 245, 238)
        Sienna = 0xA0522D,                // rgb(160, 082, 045)
        Silver = 0xC0C0C0,                // rgb(192, 192, 192)
        SkyBlue = 0x87CEEB,               // rgb(135, 206, 235)
        SlateBlue = 0x6A5ACD,             // rgb(106, 090, 205)
        SlateGray = 0x708090,             // rgb(112, 128, 144)
        Snow = 0xFFFAFA,                  // rgb(255, 250, 250)
        SpringGreen = 0x00FF7F,           // rgb(000, 255, 127)
        SteelBlue = 0x4682B4,             // rgb(070, 130, 180)
        Tan = 0xD2B48C,                   // rgb(210, 180, 140)
        Teal = 0x008080,                  // rgb(000, 128, 128)
        Thistle = 0xD8BFD8,               // rgb(216, 191, 216)
        Tomato = 0xFF6347,                // rgb(255, 099, 071)
        Turquoise = 0x40E0D0,             // rgb(064, 224, 208)
        Violet = 0xEE82EE,                // rgb(238, 130, 238)
        Wheat = 0xF5DEB3,                 // rgb(245, 222, 179)
        White = 0xFFFFFF,                 // rgb(255, 255, 255)
        WhiteSmoke = 0xF5F5F5,            // rgb(245, 245, 245)
        Yellow = 0xFFFF00,                // rgb(255, 255, 000)
        YellowGreen = 0x9ACD32,           // rgb(154, 205, 050)
    };

    using enum Preset;

    /**
     * @brief Grayscale constructor
     *
     */
    constexpr Color(uint8_t rgb = 0)
        : r(rgb), g(rgb), b(rgb) {}

    /**
     * @brief Rgb constructor
     *
     */
    constexpr Color(uint8_t r, uint8_t g, uint8_t b)
        : r(r), g(g), b(b) {}

    /**
     * @brief Preset implicit constructor
     *
     */
    constexpr Color(Preset c)
        : Color(fromHex(static_cast<uint32_t>(c))) {}

    /**
     * @brief Converter to hex value
     *
     * @param c color structure
     * @return constexpr uint32_t hex value
     */
    static constexpr uint32_t toHex(const Color c) {
        return (static_cast<uint32_t>(c.r) << 16) |
               (static_cast<uint32_t>(c.g) << 8) | c.b;
    }

    /**
     * @brief Converter from hex value
     *
     * @param hex hex uint32_t value
     * @return constexpr Color value
     */
    static constexpr Color fromHex(const uint32_t hex) {
        return Color(((hex >> 16) & 0xFF),
                     ((hex >> 8) & 0xFF),
                     (hex & 0xFF));
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os, const Color& col) {
        my::printf(os, "rgb({}, {}, {})", +col.r, +col.g, +col.b);
        return os;
    }

    constexpr bool operator==(uint32_t hex) { return toHex(*this) == hex; }
    constexpr bool operator!=(uint32_t hex) { return !(*this == hex); }

    uint8_t r, g, b;
};

}  // namespace my

#endif  // MY_COLOR_HPP