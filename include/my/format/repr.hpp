#pragma once

#include <my/util/traits.hpp>
//
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <string>

namespace my {

inline namespace fmt {

// concepts

template <class T>
concept pair_like = requires(T val) {
    {val.first};
    {val.second};
};

template <class T, class Ostream,
          class R = std::remove_reference_t<T>>
concept representable =
    std::ranges::range<R> or
    my::is_tuple_v<R> or
    pair_like<R> or
    std::input_or_output_iterator<R> or
    my::printable<R, Ostream>;

// implicit

namespace detail {

template <class Ch, class Tr, representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _represent(std::basic_ostream<Ch, Tr>& os, const T& value);

}  // namespace detail

// presenters

struct DefaultRepresenter {
    template <class Ch, class Tr, representable<std::basic_ostream<Ch, Tr>> T>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        detail::_represent<Ch, Tr>(os, value);
    }

    template <class Ch, class Tr,
              representable<std::basic_ostream<Ch, Tr>> T>
    constexpr auto get(const T& value) const {
        std::basic_stringstream<Ch, Tr> ss;
        this->operator()(ss, value);
        return ss.str();
    }

    template <representable<std::ostream> T>
    constexpr auto operator()(const T& value) const {
        std::stringstream ss;
        this->operator()(ss, value);
        return ss.str();
    }
};

template <class Representer = DefaultRepresenter>
struct PairRepresenter {
    constexpr explicit PairRepresenter(std::string_view delim = ", ",
                                       Representer representer = {})
        : _delim(std::move(delim)),
          _represent(std::move(representer)) {
    }

    template <pair_like T, class Ch, class Tr>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        _represent(os, value.first);
        os << _delim;
        _represent(os, value.second);
    }

   private:
    const std::string_view _delim;
    Representer _represent;
};

constexpr PairRepresenter<> pairRepresent;

template <class Representer = DefaultRepresenter>
struct RangeRepresenter {
   public:
    constexpr explicit RangeRepresenter(std::string_view delim = ", ",
                                        Representer representer = {})
        : _delim(std::move(delim)),
          _represent(std::move(representer)) {
    }

    template <std::ranges::range Range, class Ch, class Tr>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const Range& value) const {
        auto first = std::ranges::begin(value);
        auto last = std::ranges::end(value);

        if (first == last) return;

        auto it = first;
        _represent(os, *it);

        for (++it; it != last; ++it) {
            os << _delim;
            _represent(os, *it);
        }
    }

   private:
    const std::string_view _delim;
    Representer _represent;
};

constexpr RangeRepresenter<> rangeRepresent;

template <class Representer = DefaultRepresenter>
struct TupleRepresenter {
   public:
    constexpr explicit TupleRepresenter(std::string_view delim = ", ",
                                        Representer representer = {})
        : _delim(std::move(delim)),
          _represent(std::move(representer)) {
    }

    template <class Ch, class Tr, class... Args>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const std::tuple<Args...>& value) const {
        std::apply(
            [&os, this](const auto& arg,
                        const auto&... args) {
                _represent(os, arg);
                ((os << _delim, _represent(os, args)), ...);
            },
            value);
    }

   private:
    const std::string_view _delim;
    Representer _represent;
};

constexpr TupleRepresenter<> tupleRepresent;

namespace detail {

template <class Ch, class Tr, representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _represent(std::basic_ostream<Ch, Tr>& os, const T& value) {
    if constexpr (my::printable<T>) {
        os << value;
    } else if constexpr (std::ranges::range<T>) {
        rangeRepresent(os, value);
    } else if constexpr (pair_like<T>) {
        pairRepresent(os, value);
    } else if constexpr (my::is_tuple_v<T>) {
        tupleRepresent(os, value);
    } else if constexpr (std::input_or_output_iterator<T>) {
        _represent(os, *value);
    }
    return;
}

}  // namespace detail

constexpr DefaultRepresenter represent;

}  // namespace fmt

};  // namespace my