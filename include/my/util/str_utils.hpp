#pragma once
#ifndef MY_STRING_UTILS_HPP
#define MY_STRING_UTILS_HPP

#include <my/util/algorithm.hpp>
#include <my/util/num_parser.hpp>
//
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

namespace my {

template <class StringType,
          class DelimType,
          class Container = std::vector<StringType>,
          class InserterT = my::inserter_for_t<Container>>
inline Container split(const StringType &what,
                       const DelimType &delim,
                       InserterT insert = InserterT{}) {
    using CharT = typename StringType::value_type;

    if constexpr (std::is_same_v<CharT, DelimType>) {
        Container result;

        std::basic_stringstream<CharT> ss(what);
        StringType s;

        while (std::getline(ss, s, delim)) {
            insert(result, s);
        }

        return result;

    } else {
        using std::empty;
        using std::size;

        if (empty(delim)) {
            return my::transform<Container>(
                what,
                [](auto el) { return StringType(1, el); },
                std::move(insert));
        }

        Container result;

        size_t last = 0;
        size_t next = 0;

        while ((next = what.find(delim, last)) != StringType::npos) {
            insert(result, what.substr(last, next - last));
            last = next + 1;
        }
        insert(result, what.substr(last));

        return result;
    }
}

template <class StringType>
auto &replaceFirst(StringType &where,
                   const StringType &from, const StringType &to) {
    if (auto pos = where.find(from); pos != StringType::npos)
        where.replace(pos, from.length(), to);
    return where;
}

template <class StringType>
auto &replaceAll(StringType &where,
                 const StringType &from, const StringType &to) {
    typename StringType::size_type pos = 0;

    while ((pos = where.find(from, pos)) != StringType::npos) {
        where.replace(pos, from.size(), to);
        pos += to.size();
    }
    return where;
}

template <class StringType>
StringType repeat(const StringType &what, size_t n) {
    if (!n) return StringType{};
    if (n == 1) return what;

    StringType result;
    result.reserve(what.size() * n);

    while (n--) result.append(what);
    return result;
}

template <class StringType>
auto &padStart(StringType &what, size_t targetLen,
               typename StringType::value_type padding) {
    what.reserve(targetLen);
    return targetLen > what.size()
               ? what.insert(0, targetLen - what.size(), padding)
               : what;
}

template <class StringType>
auto &padEnd(StringType &what, size_t targetLen,
             typename StringType::value_type padding) {
    what.reserve(targetLen);
    return targetLen > what.size()
               ? what.insert(what.size(), targetLen - what.size(), padding)
               : what;
}

template <class StringType>
auto &pad(StringType &what, size_t targetLen,
          typename StringType::value_type padding) {
    if (targetLen <= what.size()) return what;

    const auto remainder = targetLen - what.size();
    const auto ceiled = (remainder + 1) / 2;
    const auto floored = remainder / 2;

    what.insert(0, ceiled, padding);
    what.insert(what.size(), floored, padding);

    return what;
}

template <class StringType>
auto &trimStart(StringType &what, typename StringType::value_type remove = ' ') {
    what.erase(what.begin(),
               std::find_if(what.begin(), what.end(),
                            [remove](auto ch) {
                                return ch != remove;
                            }));
    return what;
}

template <class StringType>
auto &trimEnd(StringType &what, typename StringType::value_type remove = ' ') {
    what.erase(std::find_if(what.rbegin(), what.rend(),
                            [remove](auto ch) {
                                return ch != remove;
                            })
                   .base(),
               what.end());
    return what;
}

template <class StringType>
auto &trim(StringType &what, typename StringType::value_type remove = ' ') {
    return trimEnd(trimStart(what, remove), remove);
}

template <class StringType>
auto &toUpper(StringType &what) {
    std::transform(what.begin(), what.end(), what.begin(),
                   [](auto ch) { return std::toupper(ch); });
    return what;
}

template <class StringType>
auto &toLower(StringType &what) {
    std::transform(what.begin(), what.end(), what.begin(),
                   [](auto ch) { return std::tolower(ch); });
    return what;
}

template <class StringType>
auto &toTitle(StringType &what) {
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
          class T>
auto toString(T &&obj) {
    static_assert(my::has_print_operator_v<T, std::basic_ostream<Ch, Tr>>);
    std::basic_stringstream<Ch, Tr, Al> ss;
    ss << std::forward<T>(obj);
    return ss.str();
}

template <class T,
          typename std::enable_if_t<(my::has_print_operator_v<T, std::ostream>), bool> = true>
auto strLength(T &&obj) {
    return my::toString(std::forward<T>(obj)).size();
}

/**
 * @brief Computes Levenshtein Distance between two strings, useful for spell checkers
 * @see https://en.wikipedia.org/wiki/Levenshtein_distance
 * @see https://rosettacode.org/wiki/Levenshtein_distance#C.2B.2B
 *
 * @tparam StringType any string like container with begin, end and size methods
 * @param lhs first string to compare
 * @param rhs second string to compare
 * @return size_t value representing Levenshtein distance between two strings
 */
template <class StringType>
size_t levDistance(const StringType &lhs, const StringType &rhs) {
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
        in._M_setstate(ios_base::badbit);
    }

    if (!extracted) err |= ios_base::failbit;
    in.setstate(err);

    return in;
}

}  // namespace my
#endif  // MY_STRING_UTILS_HPP