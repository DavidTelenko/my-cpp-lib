#pragma once
#ifndef MY_JOIN_HPP
#define MY_JOIN_HPP

#include <my/util/traits.hpp>
#include <tuple>

namespace my {

template <class T, class Ch, class Tr>
concept joinable =
    my::iterable<T> or
    my::is_tuple_v<T> or
    my::is_pair_v<T> or
    my::is_iterator_v<T> or
    my::printable<T, std::basic_ostream<Ch, Tr>>;

// forward declarations
template <class I>
class join_range;

template <class T>
class join_tuple;

template <class P>
class join_pair;

// delimiter specializations api for custom classes
template <class T>
struct delimiters {
};

template <my::iterable T>
struct delimiters<T> {
    inline constexpr static const char* open = "[";
    inline constexpr static const char* delim = ", ";
    inline constexpr static const char* close = "]";
};

template <my::iterator_concept T>
struct delimiters<T> {
    inline constexpr static const char* open = "[";
    inline constexpr static const char* delim = ", ";
    inline constexpr static const char* close = "]";
};

template <class... Ts>
struct delimiters<std::tuple<Ts...>> {
    inline constexpr static const char* open = "(";
    inline constexpr static const char* delim = ", ";
    inline constexpr static const char* close = ")";
};

template <class... Ts>
struct delimiters<std::pair<Ts...>> {
    inline constexpr static const char* open = "(";
    inline constexpr static const char* delim = ", ";
    inline constexpr static const char* close = ")";
};

/**
 * @brief
 *
 * @param val iterable, tuple or pair
 * @return join_## proxy
 */
template <class Ch, class Tr, my::joinable<Ch, Tr> T>
constexpr decltype(auto) join(const T& val) {
    if constexpr (my::printable<T, std::basic_ostream<Ch, Tr>>) {
        return val;
    } else if constexpr (my::iterable<T>) {
        using std::begin;
        using std::end;
        return join_range(begin(val), end(val));
    } else if constexpr (my::is_tuple_v<T>) {
        return join_tuple(val);
    } else if constexpr (my::is_pair_v<T>) {
        return join_pair(val);
    } else if constexpr (my::is_iterator_v<T>) {
        return join<Ch, Tr>(*val);
    }
}

template <class Ch, my::joinable<Ch, std::char_traits<Ch>> T>
inline constexpr decltype(auto) join(const T& val) {
    return join<Ch, std::char_traits<Ch>, T>(val);
}

template <my::joinable<char, std::char_traits<char>> T>
inline constexpr decltype(auto) join(const T& val) {
    return join<char, std::char_traits<char>, T>(val);
}

/**
 * @brief External manipulator for std::basic_ostream.
 * Prints newline but doesent flushes stream
 * @note can be used like so std::cout << endl
 *
 * @param os ostream&
 * @return auto& reference to os
 */
template <class Ch, class Tr>
inline auto& newline(std::basic_ostream<Ch, Tr>& os) {
    os.put(os.widen('\n'));
    return os;
}

/**
 * @brief External manipulator for std::basic_ostream.
 * Prints ' ' to the provided ostream
 * @note can be used like so std::cout << space
 *
 * @param os ostream&
 * @return auto& reference to os
 */
template <class Ch, class Tr>
inline auto& space(std::basic_ostream<Ch, Tr>& os) {
    os.put(os.widen(' '));
    return os;
}

/**
 * @brief External manipulator for std::basic_ostream.
 * Prints ", " to the provided ostream
 * @note can be used like so std::cout << comma
 *
 * @param os ostream&
 * @return auto& reference to os
 */
template <class Ch, class Tr>
inline auto& comma(std::basic_ostream<Ch, Tr>& os) {
    os.put(os.widen(','));
    os << space;
    return os;
}

/**
 * @brief Range proxy to provide formatted output to console.
 * Providing data, delimiter, open and close bracket you effectively get next pattern:
 * {open}{el}{delim}...{close}
 *
 * @tparam It InputIterator
 */
template <class It>
class join_range {
    It b, e;

   public:
    using value_type = It;

    explicit constexpr join_range(It begin, It end)
        : b(std::move(begin)),
          e(std::move(end)) {}

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const join_range& rv) {
        return rv.print(os);
    }

    template <class Ch, class Tr>
    constexpr operator std::basic_string<Ch, Tr>() const {
        std::basic_ostringstream<Ch, Tr> os;
        return print(os).str();
    }

    template <class Ch = char, class Tr = std::char_traits<Ch>>
    constexpr auto toString() const {
        std::basic_ostringstream<Ch, Tr> os;
        return print(os).str();
    }

   private:
    template <class Ch, class Tr>
    constexpr auto& print(std::basic_ostream<Ch, Tr>& os) const {
        os << delimiters<It>::open;

        if (b == e) {
            os << delimiters<It>::close;
            return os;
        }

        auto it = b;
        for (auto prev = std::prev(e); it != prev; ++it)
            os << join<Ch, Tr>(*it) << delimiters<It>::delim;
        os << join<Ch, Tr>(*it);

        os << delimiters<It>::close;
        return os;
    }
};

/**
 * @brief Tuple proxy to provide formatted output to console.
 * Providing data, delimiter, open and close bracket you effectively get next pattern:
 * {open}{el}{delim}...{close}
 *
 * @tparam T TupleType
 */
template <class T>
class join_tuple {
    const T& tuple;

   public:
    using value_type = T;

    explicit constexpr join_tuple(const T& rng) : tuple(rng) {}

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const join_tuple& rv) {
        return rv.print(os);
    }

    template <class Ch, class Tr>
    constexpr operator std::basic_string<Ch, Tr>() const {
        std::basic_ostringstream<Ch, Tr> os;
        return print(os).str();
    }

    template <class Ch = char, class Tr = std::char_traits<Ch>>
    inline constexpr auto toString() const {
        std::basic_ostringstream<Ch, Tr> os;
        return print(os).str();
    }

   private:
    template <class Ch, class Tr>
    auto& print(std::basic_ostream<Ch, Tr>& os) const {
        os << delimiters<T>::open;
        std::apply(
            tuple, [&os]<class Arg, class... Args>(Arg && arg, Args && ... args) {
                os << join<Ch, Tr>(std::forward<Arg>(arg));
                ((os << delimiters<T>::delim
                     << join<Ch, Tr>(std::forward<Args>(args))),
                 ...);
            });
        os << delimiters<T>::close;
        return os;
    }
};

/**
 * @brief Pair proxy to provide formatted output to console.
 * Providing data, delimiter, open and close bracket you effectively get next pattern:
 * {open}{el}{delim}...{close}
 *
 * @tparam T PairType
 */
template <class T>
class join_pair {
    const T& pair;

   public:
    using value_type = T;

    explicit constexpr join_pair(const T& rng) : pair(rng) {}

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const join_pair& rv) {
        return rv.print(os);
    }

    template <class Ch, class Tr>
    constexpr operator std::basic_string<Ch, Tr>() const {
        std::basic_ostringstream<Ch, Tr> os;
        return print(os).str();
    }

    template <class Ch = char, class Tr = std::char_traits<Ch>>
    constexpr auto toString() const {
        std::basic_ostringstream<Ch, Tr> os;
        return print(os).str();
    }

   private:
    template <class Ch, class Tr>
    auto& print(std::basic_ostream<Ch, Tr>& os) const {
        os << delimiters<T>::open
           << join<Ch, Tr>(pair.first)
           << delimiters<T>::delim
           << join<Ch, Tr>(pair.second)
           << delimiters<T>::close;
        return os;
    }
};

}  // namespace my

#endif  // MY_JOIN_HPP