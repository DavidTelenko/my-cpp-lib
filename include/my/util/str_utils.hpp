#pragma once
#ifndef MY_STRING_UTILS_HPP
#define MY_STRING_UTILS_HPP

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

namespace my {

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
inline std::vector<String> split(const String &what,
                                 const String &delim) {
    std::vector<String> result;

    size_t last = 0;
    size_t next = 0;

    while ((next = what.find(delim, last)) != String::npos) {
        insert(result, what.substr(last, next - last));
        last = next + 1;
    }
    result.push_back(what.substr(last));

    return result;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &replaceFirst(String &where, const String &from, const String &to) {
    if (auto pos = where.find(from); pos != String::npos)
        where.replace(pos, from.length(), to);
    return where;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &replaceAll(String &where,
                 const String &from, const String &to) {
    typename String::size_type pos = 0;

    while ((pos = where.find(from, pos)) != String::npos) {
        where.replace(pos, from.size(), to);
        pos += to.size();
    }
    return where;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
String repeat(const String &what, size_t n) {
    if (!n) return String{};
    if (n == 1) return what;

    String result;
    result.reserve(what.size() * n);

    while (n--) result.append(what);
    return result;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &padStart(String &what, size_t targetLen,
               typename String::value_type padding) {
    what.reserve(targetLen);
    return targetLen > what.size()
               ? what.insert(0, targetLen - what.size(), padding)
               : what;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &padEnd(String &what, size_t targetLen,
             typename String::value_type padding) {
    what.reserve(targetLen);
    return targetLen > what.size()
               ? what.insert(what.size(), targetLen - what.size(), padding)
               : what;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &pad(String &what, size_t targetLen,
          typename String::value_type padding) {
    if (targetLen <= what.size()) return what;

    const auto remainder = targetLen - what.size();
    const auto ceiled = (remainder + 1) / 2;
    const auto floored = remainder / 2;

    what.insert(0, ceiled, padding);
    what.insert(what.size(), floored, padding);

    return what;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &trimStart(String &what, typename String::value_type remove = ' ') {
    what.erase(what.begin(),
               std::find_if(what.begin(), what.end(),
                            [remove](auto ch) {
                                return ch != remove;
                            }));
    return what;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &trimEnd(String &what, typename String::value_type remove = ' ') {
    what.erase(std::find_if(what.rbegin(), what.rend(),
                            [remove](auto ch) {
                                return ch != remove;
                            })
                   .base(),
               what.end());
    return what;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &trim(String &what, typename String::value_type remove = ' ') {
    return trimEnd(trimStart(what, remove), remove);
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &toUpper(String &what) {
    std::transform(what.begin(), what.end(), what.begin(),
                   [](auto ch) { return std::toupper(ch); });
    return what;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &toLower(String &what) {
    std::transform(what.begin(), what.end(), what.begin(),
                   [](auto ch) { return std::tolower(ch); });
    return what;
}

template <class Ch, class Tr, class Al,
          class String = std::basic_string<Ch, Tr, Al>>
auto &toTitle(String &what) {
    bool up = true;
    for (auto &el : what) {
        if (up) el = std::toupper(el);
        up = (el == ' ');
    }
    return what;
}

template <class Ch = char,
          class Tr = std::char_traits<Ch>,
          class Al = std::allocator<Ch>,
          class T, class Ostream = std::basic_stringstream<Ch, Tr, Al>>
auto toString(T &&obj) requires my::printable<T, Ostream> {
    Ostream ss;
    ss << std::forward<T>(obj);
    return ss.str();
}

template <class T>
auto strLength(T &&obj) { return my::toString(std::forward<T>(obj)).size(); }

/**
 * @brief Computes Levenshtein Distance between two strings, useful for spell checkers
 * @see https://en.wikipedia.org/wiki/Levenshtein_distance
 * @see https://rosettacode.org/wiki/Levenshtein_distance#C.2B.2B
 *
 * @tparam String any string like container with begin, end and size methods
 * @param lhs first string to compare
 * @param rhs second string to compare
 * @return size_t value representing Levenshtein distance between two strings
 */
template <std::ranges::range String>
size_t levDistance(const String &lhs, const String &rhs) {
    const size_t m = lhs.size();
    const size_t n = rhs.size();

    if (!m) return n;
    if (!n) return m;

    std::vector<size_t> costs(n + 1);
    std::iota(costs.begin(), costs.end(), 0);

    size_t i = 0;
    for (auto &&c1 : lhs) {
        costs[0] = i + 1;
        size_t corner = i;

        size_t j = 0;
        for (auto c2 : rhs) {
            size_t upper = costs[j + 1];
            costs[j + 1] =
                (c1 == c2)
                    ? corner
                    : 1 + std::min(std::min(upper, corner), costs[j]);
            corner = upper;
            ++j;
        }
        ++i;
    }

    return costs[n];
}

/**
 * @brief Parses number to specified precision.
 *
 * @tparam Number of any arithmetic type
 * @param str string_view of number
 * @return Parsed Number as an std::optional
 */
template <my::arithmetic Number>
inline constexpr std::optional<Number> parse(std::string_view str) {
    std::istringstream ss(str.data());
    Number result;
    ss >> std::boolalpha >> result;
    return ss.fail() ? std::nullopt : result;
}

template <typename Ch, typename Tr, typename Al, class Predicate>
std::basic_istream<Ch, Tr> &
getline(std::basic_istream<Ch, Tr> &in,
        std::basic_string<Ch, Tr, Al> &str, Predicate predicate) {
    using istream_type = std::basic_istream<Ch, Tr>;
    using string_type = std::basic_string<Ch, Tr, Al>;
    using ios_base = typename istream_type::ios_base;
    using int_type = typename istream_type::int_type;
    using size_type = typename string_type::size_type;

    size_type extracted = 0;
    const size_type n = str.max_size();
    typename std::ios_base::iostate err = ios_base::goodbit;
    typename istream_type::sentry cerb(in, true);

    if (!cerb) {
        err |= ios_base::failbit;
        in.setstate(err);
        return in;
    }

    try {
        str.erase();
        const int_type eof = Tr::eof();

        int_type c = in.rdbuf()->sgetc();
        while (extracted < n and
               !Tr::eq_int_type(c, eof) and
               !std::invoke(predicate, Tr::to_char_type(c))) {
            str += Tr::to_char_type(c);
            ++extracted;
            c = in.rdbuf()->snextc();
        }

        if (Tr::eq_int_type(c, eof)) {
            err |= ios_base::eofbit;
        } else if (std::invoke(predicate, Tr::to_char_type(c))) {
            ++extracted;
            in.rdbuf()->sbumpc();
        } else {
            err |= ios_base::failbit;
        }
    } catch (...) {
        in.setstate(ios_base::badbit);
    }

    if (!extracted) err |= ios_base::failbit;
    in.setstate(err);

    return in;
}

}  // namespace my

#endif  // MY_STRING_UTILS_HPP