#pragma once

#include <my/format/repr.hpp>

namespace my {

struct JoinFunction {
    template <std::ranges::range Range, class Representer = DefaultRepresenter>
    constexpr auto
    operator()(const Range& range,
               std::string_view delim = ", ",
               Representer representer = {}) const {
        return my::RangeRepresenter(delim, representer).view(range);
    }

    template <class Representer, class... Args>
    constexpr auto operator()(const std::tuple<Args...>& tuple,
                              std::string_view delim,
                              Representer representer) const {
        return my::TupleRepresenter(delim, representer).view(tuple);
    }

    template <class... Args>
    constexpr auto operator()(const std::tuple<Args...>& tuple,
                              std::string_view delim = ", ") const {
        return my::TupleRepresenter(delim).view(tuple);
    }
};

constexpr JoinFunction join;

}  // namespace my