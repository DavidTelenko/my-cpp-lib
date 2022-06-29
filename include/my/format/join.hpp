#pragma once

#include <my/util/traits.hpp>
//
#include <iostream>
#include <iterator>
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
        _present(os, *it);

        for (++it; it != _last; ++it) {
            os << _delim;
            _present(os, *it);
        }
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

template <class T>
concept pair_like = requires(T val) {
    {val.first};
    {val.second};
};

struct PairPresenter {
    template <class T, class Ch, class Tr>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        os << "("
           << value.first
           << ", "
           << value.second
           << ")";
    }
};

constexpr PairPresenter pairPresent;

class TuplePresenter {
   public:
    constexpr explicit TuplePresenter(std::string_view delim = ", ")
        : _delim(std::move(delim)) {
    }

    template <class T, class Ch, class Tr>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        std::apply([&os, this](const auto& arg,
                               const auto&... args) {
            os << arg;
            ((os << _delim << args), ...);
        },
                   value);
    }

   private:
    const std::string_view _delim;
};

constexpr TuplePresenter tuplePresent;

struct DefaultPresenter {
    template <class T, class Ch, class Tr>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const
        requires(my::printable<T, std::basic_ostream<Ch, Tr>> or
                 std::ranges::range<T>) {
        if constexpr (std::ranges::range<T>)
            os << join(value);
        else if constexpr (pair_like<T>)
            pairPresent(os, value);
        // else if constexpr (tuple_like<T>)
        //     tuplePresent(os, value);
        else
            os << value;
    }
};

constexpr DefaultPresenter present;

}  // namespace my
