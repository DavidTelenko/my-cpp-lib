#pragma once
#ifndef MY_COLOR_PRESETS_HPP
#define MY_COLOR_PRESETS_HPP

#include <stdint.h>

namespace my {

enum class color : uint32_t {
    AliceBlue = 0xF0F8FF,             // Rgb(240, 248, 255)
    AntiqueWhite = 0xFAEBD7,          // Rgb(250, 235, 215)
    Aqua = 0x00FFFF,                  // Rgb(0, 255, 255)
    Aquamarine = 0x7FFFD4,            // Rgb(127, 255, 212)
    Azure = 0xF0FFFF,                 // Rgb(240, 255, 255)
    Beige = 0xF5F5DC,                 // Rgb(245, 245, 220)
    Bisque = 0xFFE4C4,                // Rgb(255, 228, 196)
    Black = 0x000000,                 // Rgb(0, 0, 0)
    BlanchedAlmond = 0xFFEBCD,        // Rgb(255, 235, 205)
    Blue = 0x0000FF,                  // Rgb(0, 0, 255)
    BlueViolet = 0x8A2BE2,            // Rgb(138, 43, 226)
    Brown = 0xA52A2A,                 // Rgb(165, 42, 42)
    BurlyWood = 0xDEB887,             // Rgb(222, 184, 135)
    CadetBlue = 0x5F9EA0,             // Rgb(95, 158, 160)
    Chartreuse = 0x7FFF00,            // Rgb(127, 255, 0)
    Chocolate = 0xD2691E,             // Rgb(210, 105, 30)
    Coral = 0xFF7F50,                 // Rgb(255, 127, 80)
    CornflowerBlue = 0x6495ED,        // Rgb(100, 149, 237)
    Cornsilk = 0xFFF8DC,              // Rgb(255, 248, 220)
    Crimson = 0xDC143C,               // Rgb(220, 20, 60)
    Cyan = 0x00FFFF,                  // Rgb(0, 255, 255)
    DarkBlue = 0x00008B,              // Rgb(0, 0, 139)
    DarkCyan = 0x008B8B,              // Rgb(0, 139, 139)
    DarkGoldenRod = 0xB8860B,         // Rgb(184, 134, 11)
    DarkGray = 0xA9A9A9,              // Rgb(169, 169, 169)
    DarkGreen = 0x006400,             // Rgb(0, 100, 0)
    DarkKhaki = 0xBDB76B,             // Rgb(189, 183, 107)
    DarkMagenta = 0x8B008B,           // Rgb(139, 0, 139)
    DarkOliveGreen = 0x556B2F,        // Rgb(85, 107, 47)
    DarkOrange = 0xFF8C00,            // Rgb(255, 140, 0)
    DarkOrchid = 0x9932CC,            // Rgb(153, 50, 204)
    DarkRed = 0x8B0000,               // Rgb(139, 0, 0)
    DarkSalmon = 0xE9967A,            // Rgb(233, 150, 122)
    DarkSeaGreen = 0x8FBC8F,          // Rgb(143, 188, 143)
    DarkSlateBlue = 0x483D8B,         // Rgb(72, 61, 139)
    DarkSlateGray = 0x2F4F4F,         // Rgb(47, 79, 79)
    DarkTurquoise = 0x00CED1,         // Rgb(0, 206, 209)
    DarkViolet = 0x9400D3,            // Rgb(148, 0, 211)
    DeepPink = 0xFF1493,              // Rgb(255, 20, 147)
    DeepSkyBlue = 0x00BFFF,           // Rgb(0, 191, 255)
    DimGray = 0x696969,               // Rgb(105, 105, 105)
    DodgerBlue = 0x1E90FF,            // Rgb(30, 144, 255)
    FireBrick = 0xB22222,             // Rgb(178, 34, 34)
    FloralWhite = 0xFFFAF0,           // Rgb(255, 250, 240)
    ForestGreen = 0x228B22,           // Rgb(34, 139, 34)
    Fuchsia = 0xFF00FF,               // Rgb(255, 0, 255)
    Gainsboro = 0xDCDCDC,             // Rgb(220, 220, 220)
    GhostWhite = 0xF8F8FF,            // Rgb(248, 248, 255)
    Gold = 0xFFD700,                  // Rgb(255, 215, 0)
    GoldenRod = 0xDAA520,             // Rgb(218, 165, 32)
    Gray = 0x808080,                  // Rgb(128, 128, 128)
    Green = 0x008000,                 // Rgb(0, 128, 0)
    GreenYellow = 0xADFF2F,           // Rgb(173, 255, 47)
    HoneyDew = 0xF0FFF0,              // Rgb(240, 255, 240)
    HotPink = 0xFF69B4,               // Rgb(255, 105, 180)
    IndianRed = 0xCD5C5C,             // Rgb(205, 92, 92)
    Indigo = 0x4B0082,                // Rgb(75, 0, 130)
    Ivory = 0xFFFFF0,                 // Rgb(255, 255, 240)
    Khaki = 0xF0E68C,                 // Rgb(240, 230, 140)
    Lavender = 0xE6E6FA,              // Rgb(230, 230, 250)
    LavenderBlush = 0xFFF0F5,         // Rgb(255, 240, 245)
    LawnGreen = 0x7CFC00,             // Rgb(124, 252, 0)
    LemonChiffon = 0xFFFACD,          // Rgb(255, 250, 205)
    LightBlue = 0xADD8E6,             // Rgb(173, 216, 230)
    LightCoral = 0xF08080,            // Rgb(240, 128, 128)
    LightCyan = 0xE0FFFF,             // Rgb(224, 255, 255)
    LightGoldenRodYellow = 0xFAFAD2,  // Rgb(250, 250, 210)
    LightGray = 0xD3D3D3,             // Rgb(211, 211, 211)
    LightGreen = 0x90EE90,            // Rgb(144, 238, 144)
    LightPink = 0xFFB6C1,             // Rgb(255, 182, 193)
    LightSalmon = 0xFFA07A,           // Rgb(255, 160, 122)
    LightSeaGreen = 0x20B2AA,         // Rgb(32, 178, 170)
    LightSkyBlue = 0x87CEFA,          // Rgb(135, 206, 250)
    LightSlateGray = 0x778899,        // Rgb(119, 136, 153)
    LightSteelBlue = 0xB0C4DE,        // Rgb(176, 196, 222)
    LightYellow = 0xFFFFE0,           // Rgb(255, 255, 224)
    Lime = 0x00FF00,                  // Rgb(0, 255, 0)
    LimeGreen = 0x32CD32,             // Rgb(50, 205, 50)
    Linen = 0xFAF0E6,                 // Rgb(250, 240, 230)
    Magenta = 0xFF00FF,               // Rgb(255, 0, 255)
    Maroon = 0x800000,                // Rgb(128, 0, 0)
    MediumAquamarine = 0x66CDAA,      // Rgb(102, 205, 170)
    MediumBlue = 0x0000CD,            // Rgb(0, 0, 205)
    MediumOrchid = 0xBA55D3,          // Rgb(186, 85, 211)
    MediumPurple = 0x9370DB,          // Rgb(147, 112, 219)
    MediumSeaGreen = 0x3CB371,        // Rgb(60, 179, 113)
    MediumSlateBlue = 0x7B68EE,       // Rgb(123, 104, 238)
    MediumSpringGreen = 0x00FA9A,     // Rgb(0, 250, 154)
    MediumTurquoise = 0x48D1CC,       // Rgb(72, 209, 204)
    MediumVioletRed = 0xC71585,       // Rgb(199, 21, 133)
    MidnightBlue = 0x191970,          // Rgb(25, 25, 112)
    MintCream = 0xF5FFFA,             // Rgb(245, 255, 250)
    MistyRose = 0xFFE4E1,             // Rgb(255, 228, 225)
    Moccasin = 0xFFE4B5,              // Rgb(255, 228, 181)
    NavajoWhite = 0xFFDEAD,           // Rgb(255, 222, 173)
    Navy = 0x000080,                  // Rgb(0, 0, 128)
    OldLace = 0xFDF5E6,               // Rgb(253, 245, 230)
    Olive = 0x808000,                 // Rgb(128, 128, 0)
    OliveDrab = 0x6B8E23,             // Rgb(107, 142, 35)
    Orange = 0xFFA500,                // Rgb(255, 165, 0)
    OrangeRed = 0xFF4500,             // Rgb(255, 69, 0)
    Orchid = 0xDA70D6,                // Rgb(218, 112, 214)
    PaleGoldenRod = 0xEEE8AA,         // Rgb(238, 232, 170)
    PaleGreen = 0x98FB98,             // Rgb(152, 251, 152)
    PaleTurquoise = 0xAFEEEE,         // Rgb(175, 238, 238)
    PaleVioletRed = 0xDB7093,         // Rgb(219, 112, 147)
    PapayaWhip = 0xFFEFD5,            // Rgb(255, 239, 213)
    PeachPuff = 0xFFDAB9,             // Rgb(255, 218, 185)
    Peru = 0xCD853F,                  // Rgb(205, 133, 63)
    Pink = 0xFFC0CB,                  // Rgb(255, 192, 203)
    Plum = 0xDDA0DD,                  // Rgb(221, 160, 221)
    PowderBlue = 0xB0E0E6,            // Rgb(176, 224, 230)
    Purple = 0x800080,                // Rgb(128, 0, 128)
    RebeccaPurple = 0x663399,         // Rgb(102, 51, 153)
    Red = 0xFF0000,                   // Rgb(255, 0, 0)
    RosyBrown = 0xBC8F8F,             // Rgb(188, 143, 143)
    RoyalBlue = 0x4169E1,             // Rgb(65, 105, 225)
    SaddleBrown = 0x8B4513,           // Rgb(139, 69, 19)
    Salmon = 0xFA8072,                // Rgb(250, 128, 114)
    SandyBrown = 0xF4A460,            // Rgb(244, 164, 96)
    SeaGreen = 0x2E8B57,              // Rgb(46, 139, 87)
    SeaShell = 0xFFF5EE,              // Rgb(255, 245, 238)
    Sienna = 0xA0522D,                // Rgb(160, 82, 45)
    Silver = 0xC0C0C0,                // Rgb(192, 192, 192)
    SkyBlue = 0x87CEEB,               // Rgb(135, 206, 235)
    SlateBlue = 0x6A5ACD,             // Rgb(106, 90, 205)
    SlateGray = 0x708090,             // Rgb(112, 128, 144)
    Snow = 0xFFFAFA,                  // Rgb(255, 250, 250)
    SpringGreen = 0x00FF7F,           // Rgb(0, 255, 127)
    SteelBlue = 0x4682B4,             // Rgb(70, 130, 180)
    Tan = 0xD2B48C,                   // Rgb(210, 180, 140)
    Teal = 0x008080,                  // Rgb(0, 128, 128)
    Thistle = 0xD8BFD8,               // Rgb(216, 191, 216)
    Tomato = 0xFF6347,                // Rgb(255, 99, 71)
    Turquoise = 0x40E0D0,             // Rgb(64, 224, 208)
    Violet = 0xEE82EE,                // Rgb(238, 130, 238)
    Wheat = 0xF5DEB3,                 // Rgb(245, 222, 179)
    White = 0xFFFFFF,                 // Rgb(255, 255, 255)
    WhiteSmoke = 0xF5F5F5,            // Rgb(245, 245, 245)
    Yellow = 0xFFFF00,                // Rgb(255, 255, 0)
    YellowGreen = 0x9ACD32,           // Rgb(154, 205, 50)
};

}  // namespace my

#endif  // MY_COLOR_PRESETS_HPP