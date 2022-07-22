#pragma once

#include <my/format/repr.hpp>

namespace my {

struct JoinFunction {
    template <std::ranges::range Range,
              my::representer_for<std::ranges::range_value_t<Range>>
                  Representer = DefaultRepresenter>
    constexpr auto operator()(const Range& range,
                              std::string_view delim = ", ",
                              Representer representer = {}) const {
        return my::RangeRepresenter(my::delim(delim), representer).view(range);
    }

    // clang-format off
    template <class Representer, class... Args>
    requires (my::representer_for<Representer, Args> and ...)
    constexpr auto operator()(const std::tuple<Args...>& tuple,
                              std::string_view delim,
                              Representer representer) const {
        return my::TupleRepresenter(my::delim(delim), representer).view(tuple);
    }
    // clang-format on

    template <class... Args>
    constexpr auto operator()(const std::tuple<Args...>& tuple,
                              std::string_view delim = ", ") const {
        return my::TupleRepresenter(my::delim(delim)).view(tuple);
    }
};

constexpr JoinFunction join;

}  // namespace my