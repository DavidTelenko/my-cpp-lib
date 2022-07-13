#pragma once

#include <my/util/meta.hpp>
//
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <string>

namespace my {

template <class T, class Ostream,
          class R = std::remove_reference_t<T>>
concept representable =
    my::printable<R, Ostream> or
    my::tuple_like<R> or
    std::ranges::range<R> or
    std::input_or_output_iterator<R>;

// implicit

namespace detail {

template <class Ch, class Tr, representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _represent(std::basic_ostream<Ch, Tr>& os, const T& value);

template <class Ch, class Tr, representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _prettyRepresent(std::basic_ostream<Ch, Tr>& os, const T& value);

// representers

template <class Representer, class T>
class _RepresentableValueView {
   public:
    constexpr explicit _RepresentableValueView(Representer represent,
                                               const T& value)
        : _represent(std::move(represent)),
          _value(value) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const _RepresentableValueView& obj) {
        obj._represent(os, obj._value);
        return os;
    }

   private:
    Representer _represent;
    const T& _value;
};

}  // namespace detail

template <class Representer>
struct BaseRepresenter {
    template <class Ch, class Tr, class T>
    constexpr void print(std::basic_ostream<Ch, Tr>& os,
                         const T& value) const {
        _derived()(os, value);
    }

    template <class T>
    constexpr void print(const T& value) const {
        print(std::cout, value);
    }

    template <class Ch, class Tr, class T>
    constexpr void println(std::basic_ostream<Ch, Tr>& os,
                           const T& value) const {
        print(os, value), os << '\n';
    }

    template <class T>
    constexpr void println(const T& value) const {
        println(std::cout, value);
    }

    template <class Ch, class Tr>
    constexpr void printf(std::basic_ostream<Ch, Tr>& os,
                          const Ch* format) const {
        os << *format;
    }

    template <class Ch, class Tr, class Arg, class... Args>
    constexpr void printf(std::basic_ostream<Ch, Tr>& os, const Ch* format,
                          Arg&& arg, Args&&... args) const {
        for (; *format != '\0'; ++format) {
            if (*format == '{') {
                if (*(format + 1) != '}') {
                    os << '{';
                    continue;
                }
                print(os, arg);
                return printf(os, (format + 2), std::forward<Args>(args)...);
            }
            os << *format;
        }
    }

    template <class Ch, class... Args>
    constexpr void printf(const Ch* format, Args&&... args) const {
        printf(std::cout, format, std::forward<Args>(args)...);
    }

    template <class Ch, class Tr, class T>
    constexpr auto get(const T& value) const {
        std::basic_stringstream<Ch, Tr> ss;
        print(ss, value);
        return ss.str();
    }

    template <class T>
    constexpr auto get(const T& value) const {
        return get<char, std::char_traits<char>>(value);
    }

    template <class T>
    constexpr auto view(const T& value) const {
        return detail::_RepresentableValueView(_derived(), value);
    }

   private:
    constexpr const auto& _derived() const noexcept {
        return static_cast<const Representer&>(*this);
    }
};

struct DefaultRepresenter
    : public BaseRepresenter<DefaultRepresenter> {
    template <class Ch, class Tr, class T>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        detail::_represent(os, value);
    }
};

struct PrettyRepresenter
    : public BaseRepresenter<PrettyRepresenter> {
    template <class Ch, class Tr, class T>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        detail::_prettyRepresent(os, value);
    }
};

template <class Representer = DefaultRepresenter>
struct RangeRepresenter
    : public BaseRepresenter<RangeRepresenter<Representer>> {
   public:
    constexpr explicit RangeRepresenter(std::string_view delim = ", ",
                                        Representer representer = {})
        : _delim(std::move(delim)),
          _represent(std::move(representer)) {
    }

    template <class Ch, class Tr,
              std::input_iterator Iter, std::sentinel_for<Iter> Sent>
    constexpr void
    operator()(std::basic_ostream<Ch, Tr>& os,
               Iter first, Sent last) const {
        if (first == last) return;

        auto it = first;
        _represent(os, *it);

        for (++it; it != last; ++it) {
            os << _delim, _represent(os, *it);
        }
    }

    template <class Ch, class Tr, std::ranges::range Range>
    constexpr void
    operator()(std::basic_ostream<Ch, Tr>& os,
               const Range& value) const {
        (*this)(os, std::ranges::begin(value), std::ranges::end(value));
    }

    template <class Ch, class Tr,
              std::input_iterator Iter, std::sentinel_for<Iter> Sent>
    constexpr void
    operator()(std::basic_ostream<Ch, Tr>& os,
               Iter first, Sent last,
               std::iter_difference_t<Iter> maxLength,
               std::iter_difference_t<Iter> lastLength = 0) const {
        const auto size = std::ranges::distance(first, last);

        if (size <= maxLength or size == maxLength + lastLength) {
            (*this)(os, first, last);
            return;
        }

        (*this)(os, first, std::ranges::next(first, maxLength));
        os << _delim << "...";

        if (!lastLength) {
            os << "(" << size - maxLength << ")";
            return;
        }

        if (size >= maxLength + lastLength) {
            os << "(" << size - maxLength - lastLength << ")";
            os << _delim;
            (*this)(os, std::ranges::prev(last, lastLength), last);
        }
    }

    template <class Ch, class Tr, std::ranges::range Range>
    constexpr void
    operator()(std::basic_ostream<Ch, Tr>& os,
               const Range& value,
               std::ranges::range_difference_t<Range> maxLength,
               std::ranges::range_difference_t<Range> lastLength = 0) const {
        (*this)(os, std::ranges::begin(value), std::ranges::end(value),
                maxLength, lastLength);
    }

   private:
    const std::string_view _delim;
    Representer _represent;
};

constexpr RangeRepresenter<DefaultRepresenter> rangeRepresent;

template <class Representer = DefaultRepresenter>
struct TupleRepresenter
    : public BaseRepresenter<TupleRepresenter<Representer>> {
   public:
    constexpr explicit TupleRepresenter(std::string_view delim = ", ",
                                        Representer representer = {})
        : _delim(std::move(delim)),
          _represent(std::move(representer)) {
    }

    template <class Ch, class Tr, class Tuple>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const Tuple& value) const {
        std::apply(
            [&os, this](const auto& arg, const auto&... args) {
                _represent(os, arg);
                ((os << _delim, _represent(os, args)), ...);
            },
            value);
    }

   private:
    const std::string_view _delim;
    Representer _represent;
};

constexpr TupleRepresenter<DefaultRepresenter> tupleRepresent;

template <class T = DefaultRepresenter>
using PairRepresenter = TupleRepresenter<T>;

constexpr PairRepresenter<DefaultRepresenter> pairRepresent;

struct PrettyOptions {
    static size_t rangeMaxLength;
    static size_t rangeMaxLengthFromEnd;
    static const char* rangeOpenDelim;
    static const char* rangeCloseDelim;
    static const char* tupleOpenDelim;
    static const char* tupleCloseDelim;
};

size_t PrettyOptions::rangeMaxLength = 5;
size_t PrettyOptions::rangeMaxLengthFromEnd = 5;
const char* PrettyOptions::rangeOpenDelim = "[";
const char* PrettyOptions::rangeCloseDelim = "]";
const char* PrettyOptions::tupleOpenDelim = "(";
const char* PrettyOptions::tupleCloseDelim = ")";

namespace detail {

template <class Ch, class Tr, representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _represent(std::basic_ostream<Ch, Tr>& os, const T& value) {
    if constexpr (my::printable<T>) {
        os << value;
    } else if constexpr (std::ranges::range<T>) {
        rangeRepresent(os, value);
    } else if constexpr (my::tuple_like<T>) {
        tupleRepresent(os, value);
    } else if constexpr (std::input_or_output_iterator<T>) {
        _represent(os, *value);
    }
    return;
}

template <class Ch, class Tr, my::printable<std::basic_ostream<Ch, Tr>> T>
constexpr void _quotedIfPossible(std::basic_ostream<Ch, Tr>& os,
                                 const T& value) {
    using std::quoted;
    constexpr bool quotedApplicable = requires(T val) {
        {quoted(val)};
    };

    if constexpr (quotedApplicable) {
        os << quoted(value);
    } else if constexpr (std::same_as<T, Ch>) {  // probably ok
        os << '\'' << value << '\'';
    } else {
        os << value;
    }
}

using PrettyRangeRepresenter = RangeRepresenter<PrettyRepresenter>;
using PrettyTupleRepresenter = TupleRepresenter<PrettyRepresenter>;

constexpr PrettyTupleRepresenter _tuplePrettyRepresent;
constexpr PrettyRangeRepresenter _rangeOfValuesPrettyRepresent;
constexpr PrettyRangeRepresenter _rangeOfRangesOrTuplesRepresent(",\n");

template <class Ch, class Tr, representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _prettyRepresent(std::basic_ostream<Ch, Tr>& os,
                                const T& value) {
    if constexpr (my::printable<T>) {
        _quotedIfPossible(os, value);
    } else if constexpr (std::ranges::range<T>) {
        using value_t = std::ranges::range_value_t<T>;
        os << PrettyOptions::rangeOpenDelim;
        if constexpr (std::ranges::range<value_t> or my::tuple_like<value_t>) {
            _rangeOfRangesOrTuplesRepresent(
                os, value,
                PrettyOptions::rangeMaxLength,
                PrettyOptions::rangeMaxLengthFromEnd);
        } else {
            _rangeOfValuesPrettyRepresent(
                os, value,
                PrettyOptions::rangeMaxLength,
                PrettyOptions::rangeMaxLengthFromEnd);
        }
        os << PrettyOptions::rangeCloseDelim;
    } else if constexpr (my::tuple_like<T>) {
        os << PrettyOptions::tupleOpenDelim;
        _tuplePrettyRepresent(os, value);
        os << PrettyOptions::tupleOpenDelim;
    } else if constexpr (std::input_or_output_iterator<T>) {
        _prettyRepresent(os, *value);
    }
}

}  // namespace detail

constexpr DefaultRepresenter represent;
constexpr PrettyRepresenter pretty;

};  // namespace my