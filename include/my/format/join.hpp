#pragma once
#ifndef MY_JOIN_HPP
#define MY_JOIN_HPP

#include <my/format/delimiters.hpp>
//
#include <tuple>

namespace my {

template <class T, class Ch, class Tr,
          class R = std::remove_reference_t<T>>
concept joinable =
    my::iterable<R> or
    my::is_tuple_v<R> or
    my::is_pair_v<R> or
    std::input_or_output_iterator<R> or
    my::printable<R, std::basic_ostream<Ch, Tr>>;

// forward declarations
template <class I>
class join_range;

template <class T>
class join_tuple;

template <class P>
class join_pair;

/**
 * @brief
 *
 * @param val iterable, tuple or pair
 * @return join_## proxy
 */
template <class Ch, class Tr, class T, class R = std::remove_reference_t<T>>
requires my::joinable<R, Ch, Tr>
constexpr decltype(auto) join(T&& val) {
    if constexpr (my::printable<R, std::basic_ostream<Ch, Tr>>) {
        return std::forward<R>(val);
    } else if constexpr (my::iterable<R>) {
        using std::begin;
        using std::end;
        return join_range(begin(std::forward<R>(val)),
                          end(std::forward<R>(val)));
    } else if constexpr (my::is_tuple_v<R>) {
        return join_tuple(std::forward<R>(val));
    } else if constexpr (my::is_pair_v<R>) {
        return join_pair(std::forward<R>(val));
    } else if constexpr (std::input_or_output_iterator<R>) {
        return join<Ch, Tr>(*std::forward<R>(val));
    }
}

template <class Ch, class T, class R = std::remove_reference_t<T>>
constexpr decltype(auto) join(T&& val) requires(
    my::joinable<R, Ch, std::char_traits<Ch>> and
    not std::same_as<Ch, T>) {
    return join<Ch, std::char_traits<Ch>, R>(std::forward<R>(val));
}

template <class T, class R = std::remove_reference_t<T>>
requires my::joinable<R, char, std::char_traits<char>>
constexpr decltype(auto) join(T&& val) {
    return join<char, std::char_traits<char>, R>(std::forward<R>(val));
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
   public:
    using value_type = It;

    constexpr explicit join_range(It begin, It end)
        : _first(std::move(begin)),
          _last(std::move(end)) {}

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
    It _first, _last;

    template <class Ch, class Tr>
    constexpr auto& print(std::basic_ostream<Ch, Tr>& os) const {
        os << delimiters<It>::open;

        if (_first == _last) {
            os << delimiters<It>::close;
            return os;
        }

        auto it = _first;
        for (auto prev = std::prev(_last); it != prev; ++it)
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
   public:
    using value_type = T;

    constexpr explicit join_tuple(const T& rng) : _tuple(rng) {}

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
    constexpr auto toString() const {
        std::basic_ostringstream<Ch, Tr> os;
        return print(os).str();
    }

   private:
    const T& _tuple;

    template <class Ch, class Tr>
    auto& print(std::basic_ostream<Ch, Tr>& os) const {
        os << delimiters<T>::open;
        std::apply(
            _tuple, [&os]<class Arg, class... Args>(Arg&& arg,
                                                    Args&&... args) {
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
   public:
    using value_type = T;

    constexpr explicit join_pair(const T& rng) : _pair(rng) {}

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
    const T& _pair;

    template <class Ch, class Tr>
    auto& print(std::basic_ostream<Ch, Tr>& os) const {
        os << delimiters<T>::open
           << join<Ch, Tr>(_pair.first)
           << delimiters<T>::delim
           << join<Ch, Tr>(_pair.second)
           << delimiters<T>::close;
        return os;
    }
};

}  // namespace my

#endif  // MY_JOIN_HPP