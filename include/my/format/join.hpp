#pragma once

#include <my/format/repr.hpp>

namespace my {

inline namespace fmt {

template <std::input_iterator It, class Representer>
class JoinedRangeView {
   public:
    constexpr explicit JoinedRangeView(It first, It last,
                                       std::string_view delim,
                                       Representer representer)
        : _first(std::move(first)),
          _last(std::move(last)),
          _delim(std::move(delim)),
          _represent(std::move(representer)) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const JoinedRangeView& joined) {
        joined.print(os);
        return os;
    }

    template <class Ch, class Tr>
    constexpr void print(std::basic_ostream<Ch, Tr>& os) const {
        if (_first == _last) return;

        auto it = _first;
        _represent(os, *it);

        for (++it; it != _last; ++it) {
            os << _delim;
            _represent(os, *it);
        }
    }

   private:
    It _first, _last;
    std::string_view _delim;
    my::make_member_function_t<Representer> _represent;
};

template <class Representer, class... Args>
class JoinedTupleView {
   public:
    constexpr explicit JoinedTupleView(const std::tuple<Args...>& tuple,
                                       std::string_view delim,
                                       Representer representer)
        : _tuple(tuple),
          _delim(std::move(delim)),
          _represent(std::move(representer)) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const JoinedTupleView& joined) {
        joined.print(os);
        return os;
    }

    template <class Ch, class Tr>
    constexpr void print(std::basic_ostream<Ch, Tr>& os) const {
        using namespace detail;
        std::apply(
            [&os, this](const auto& arg,
                        const auto&... args) {
                _represent(os, arg);
                ((os << _delim, _represent(os, args)), ...);
            },
            _tuple);
    }

   private:
    const std::tuple<Args...>& _tuple;
    std::string_view _delim;
    my::make_member_function_t<Representer> _represent;
};

struct JoinFunction {
    template <std::ranges::range Range, class Representer = DefaultRepresenter>
    constexpr auto
    operator()(const Range& range,
               std::string_view delim = ", ",
               Representer representer = {}) const {
        return JoinedRangeView(std::ranges::begin(range),
                               std::ranges::end(range),
                               delim, representer);
    }

    template <class Representer, class... Args>
    constexpr auto operator()(const std::tuple<Args...>& tuple,
                              std::string_view delim,
                              Representer representer) const {
        return JoinedTupleView(tuple, delim, representer);
    }

    template <class... Args>
    constexpr auto operator()(const std::tuple<Args...>& tuple,
                              std::string_view delim = ", ") const {
        return JoinedTupleView(tuple, delim, my::represent);
    }
};

constexpr JoinFunction join;

}  // namespace fmt

}  // namespace my