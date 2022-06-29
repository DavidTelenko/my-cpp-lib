#pragma once

#include <iostream>
#include <iterator>
#include <my/util/traits.hpp>
#include <ranges>
#include <string>

namespace my {

template <std::input_iterator It, class Presenter>
class JoinedRange {
   public:
    constexpr explicit JoinedRange(It first, It last,
                                   std::string_view delim,
                                   Presenter present)
        : _first(std::move(first)),
          _last(std::move(last)),
          _delim(std::move(delim)),
          _present(std::move(present)) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const JoinedRange& joined) {
        joined.print(os);
        return os;
    }

   private:
    template <class Ch, class Tr>
    constexpr void print(std::basic_ostream<Ch, Tr>& os) const {
        using namespace detail;

        if (_first == _last) return;

        auto it = _first;
        for (auto prev = std::ranges::prev(_last); it != prev; ++it) {
            os << _present(*it) << _delim;
        }

        os << _present(*it);
    }

   private:
    It _first, _last;
    std::string_view _delim;
    my::make_member_function_t<Presenter> _present;
};

class DefaultPresenter;

struct JoinFunction {
    template <std::ranges::range Range, class Presenter = DefaultPresenter>
    constexpr auto operator()(const Range& range,
                              std::string_view delim = ", ",
                              Presenter present = {}) const {
        namespace rng = std::ranges;
        return JoinedRange(rng::begin(range), rng::end(range), delim, present);
    }
};

constexpr JoinFunction join;

struct DefaultPresenter {
    template <class T, class R = std::remove_reference_t<T>>
    constexpr auto operator()(T&& value) const requires(
        my::printable<R> or
        std::ranges::range<R>) {
        if constexpr (std::ranges::range<R>) {
            return join(value);
        } else {
            return std::forward<R>(value);
        }
    }
};

}  // namespace my
