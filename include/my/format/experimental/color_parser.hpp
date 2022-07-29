#pragma once

#include <my/format/color.hpp>
#include <my/util/str_utils.hpp>

namespace my::experimental {

class ColorParser {
    inline static const std::map<std::string, my::Color> colors{
        {"red", my::Color::Red},
        {"green", my::Color::Green},
        {"blue", my::Color::Blue},
        {"orange", my::Color::Orange},
        {"yellow", my::Color::Yellow},
        {"cyan", my::Color::Cyan},
        {"purple", my::Color::Purple},
        {"magenta", my::Color::Magenta},
        {"brown", my::Color::Brown},
        {"black", my::Color::Black},
        {"gray", my::Color::Gray},
        {"white", my::Color::White},
    };

    inline static std::map<std::string, my::Color> userColors{};

    static uint32_t parseHex(const std::string& buf) {
        uint32_t result{};

        for (auto&& el : buf) {
            if (not std::isxdigit(el)) {
                return 0xFFFFFF + 1;  // invalid color
            }
        }

        std::stringstream ss;
        ss << std::hex << buf;
        ss >> result;

        return result;
    }

    static auto getColor(const std::string& key, bool& validity) {
        my::Color color;
        if (auto uit = userColors.find(key); uit != userColors.end()) {
            color = uit->second;
        } else if (auto cit = colors.find(key); cit != colors.end()) {
            color = cit->second;
        } else if (auto hex = parseHex(key); hex <= 0xFFFFFF) {
            color = hex;
        } else {
            validity = false;
        }
        return color;
    }

   public:
    static void defineColor(const std::string& key, const my::Color color) {
        if (key.empty()) return;
        userColors[key] = color;
    }

    static auto parse(std::ostream& os, const std::string& content) {
        for (size_t i = 0; i < content.size(); i++) {
            const auto el = content[i];
            if (el == '[') {
                if (content[i + 1] != '#') {
                    os << '[';
                    continue;
                }

                std::string fg;
                std::string bg;
                bool willBeBg = false;
                bool validity = true;

                size_t j = i + 1;
                for (; j < content.size(); j++) {
                    const auto tel = content[j];
                    if (tel == ':') break;
                    if (tel == ',') {
                        willBeBg = true;
                        break;
                    }
                    fg.push_back(tel);
                }

                if (willBeBg) {
                    for (j++; j < content.size(); j++) {
                        const auto tel = content[j];
                        if (tel == '#') break;
                        if (!std::isblank(tel)) {
                            validity = false;
                            my::printf("{}\n", tel);
                            break;
                        }
                    }

                    if (!validity) {
                        os << '[';
                        continue;
                    }

                    for (; j < content.size(); j++) {
                        const auto tel = content[j];
                        if (tel == ':') break;
                        bg.push_back(tel);
                    }
                }

                bool bgValid = true;
                auto foreground = getColor(my::trim(fg).erase(0, 1), validity),
                     background = getColor(my::trim(bg).erase(0, 1), bgValid);

                if (!validity or (willBeBg and !bgValid)) {
                    os << '[';
                    continue;
                }

                if (willBeBg) {
                    my::setcol(os, foreground, background);
                } else {
                    my::setfg(os, foreground);
                }

                size_t opened = 0;
                for (j++; j < content.size(); j++) {
                    const auto tel = content[j];
                    if (tel == '[') opened++;
                    if (tel == ']') {
                        if (not opened) break;
                        opened--;
                    }
                    os << tel;
                }
                // FIXME escaping ']'
                // bool escaped = false;
                // for (j++; j < content.size(); j++) {
                //     const auto tel = content[j];
                //     if (tel == '\\') {
                //         escaped = true;
                //         continue;
                //     }
                //     if (tel == ']') {
                //         if (not escaped) break;
                //         escaped = false;
                //     } else if (escaped) {
                //         os << '\\';
                //         escaped = false;
                //     }

                //     os << tel;
                // }

                my::resetcol(os);

                i = j;
                continue;
            }
            os << el;
        }
    }
};

/**
 * @brief Prints formatted text with color using small substitution parsing.
 *
 * # Parsing grammar
 * `color      ::= user_color | inner_color | hex_value`
 * `background ::= color`
 * `foreground ::= color`
 * `content    ::= text | text"{}"`
 * `space      ::= " "+`
 * `expression ::= "[#" foreground ["," [space] "#" background] ":" content "]"`
 *
 * # Example
 * ```
 * my::ColorParser::defineColor("gold", my::Color::Gold); // defining our own color
 * my::printcol("[#000000, #gold:[The answer is]] [#red:{}]", 42);
 * // escaping ']' with even amount of opened and closed []
 * ```
 *
 * @tparam Args any types
 * @param format format string
 * @param args {} anchors gets replaced with args
 */
template <class... Args>
auto printcol(std::ostream& os, const char* format, Args&&... args) {
    std::stringstream ss;
    my::printf(ss, format, args...);
    ColorParser::parse(os, ss.str());
};

/**
 * @brief Prints formatted text with color using substitution parsing.
 *
 * @tparam Args any types
 * @param format format string
 * @param args {} anchors gets replaced with args
 */
template <class... Args>
auto printcol(const char* format, Args&&... args) {
    my::printcol(std::cout, format, args...);
};

template <class... Args>
auto formatcol(const char* format, Args&&... args) {
    std::stringstream ss;
    my::printcol(ss, format, args...);
    return ss.str();
}

}  // namespace my