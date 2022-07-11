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

template <class Representer>
struct _BaseRepresenter {
    template <class Ch, class Tr, class T>
    constexpr auto get(const T& value) const {
        std::basic_stringstream<Ch, Tr> ss;
        (static_cast<const Representer&>(*this))(ss, value);
        return ss.str();
    }

    template <class T>
    constexpr auto get(const T& value) const {
        std::stringstream ss;
        (static_cast<const Representer&>(*this))(ss, value);
        return ss.str();
    }

    template <class T>
    constexpr auto view(const T& value) const {
        return detail::_RepresentableValueView(
            static_cast<const Representer&>(*this), value);
    }
};

}  // namespace detail

struct DefaultRepresenter
    : public detail::_BaseRepresenter<DefaultRepresenter> {
    template <class Ch, class Tr, class T>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        detail::_represent(os, value);
    }
};

struct PrettyRepresenter
    : public detail::_BaseRepresenter<PrettyRepresenter> {
    template <class Ch, class Tr, class T>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        detail::_prettyRepresent(os, value);
    }
};

template <class Representer = DefaultRepresenter>
struct RangeRepresenter
    : public detail::_BaseRepresenter<RangeRepresenter<Representer>> {
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

        for (++it; it != last;
             ++it) {
            os << _delim;
            _represent(os, *it);
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

        if (size <= maxLength) {
            (*this)(os, first, last);
            return;
        }

        (*this)(os, first, first + maxLength);
        os << _delim << "...";

        if (!lastLength) return;

        if (size >= maxLength + lastLength) {
            os << _delim;
            (*this)(os, last - lastLength, last);
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
constexpr RangeRepresenter<PrettyRepresenter> rangePrettyRepresent;

template <class Representer = DefaultRepresenter>
struct TupleRepresenter
    : public detail::_BaseRepresenter<TupleRepresenter<Representer>> {
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
constexpr TupleRepresenter<PrettyRepresenter> tuplePrettyRepresent;

template <class T = DefaultRepresenter>
using PairRepresenter = TupleRepresenter<T>;

constexpr PairRepresenter<DefaultRepresenter> pairRepresent;
constexpr PairRepresenter<PrettyRepresenter> pairPrettyRepresent;

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

template <class Ch, class Tr, representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _prettyRepresent(std::basic_ostream<Ch, Tr>& os,
                                const T& value) {
    if constexpr (my::printable<T>) {
        _quotedIfPossible(os, value);
    } else if constexpr (std::ranges::range<T>) {
        os << '[';
        rangePrettyRepresent(os, value, 5, 5);  // maybe print amount between
        os << ']' << '\n';
        // TODO newlines after nested ranges/tuples
    } else if constexpr (my::tuple_like<T>) {
        os << '(';
        tuplePrettyRepresent(os, value);
        os << ')';
        // TODO newlines after nested tuples/ranges
    } else if constexpr (std::input_or_output_iterator<T>) {
        _prettyRepresent(os, *value);
    }
    return;
}

}  // namespace detail

constexpr DefaultRepresenter represent;
constexpr PrettyRepresenter pretty;

};  // namespace my