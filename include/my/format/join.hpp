#pragma once

#include <my/util/traits.hpp>
//
#include <iostream>
#include <iterator>
#include <ranges>
#include <string>

namespace my {

template <class T>
concept pair_like = requires(T val) {
    {val.first};
    {val.second};
};

template <class T, class Ch, class Tr,
          class R = std::remove_reference_t<T>>
concept default_presentable =
    std::ranges::range<R> or
    my::is_tuple_v<R> or
    pair_like<R> or
    std::input_or_output_iterator<R> or
    my::printable<R, std::basic_ostream<Ch, Tr>>;

template <class Ch, class Tr, default_presentable<Ch, Tr> T>
constexpr void present(std::basic_ostream<Ch, Tr>& os, const T& value);

struct DefaultPresenter {
    template <class Ch, class Tr, default_presentable<Ch, Tr> T>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const { present(os, value); }
};

template <std::input_iterator It, class Presenter>
class JoinedRangeView {
   public:
    constexpr explicit JoinedRangeView(It first, It last,
                                       std::string_view delim,
                                       Presenter presenter)
        : _first(std::move(first)),
          _last(std::move(last)),
          _delim(std::move(delim)),
          _present(std::move(presenter)) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const JoinedRangeView& joined) {
        joined.print(os);
        return os;
    }

   private:
    template <class Ch, class Tr>
    constexpr void print(std::basic_ostream<Ch, Tr>& os) const {
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

template <class Presenter, class... Args>
class JoinedTupleView {
   public:
    constexpr explicit JoinedTupleView(const std::tuple<Args...>& tuple,
                                       std::string_view delim,
                                       Presenter presenter)
        : _tuple(tuple),
          _delim(std::move(delim)),
          _present(std::move(presenter)) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const JoinedTupleView& joined) {
        joined.print(os);
        return os;
    }

   private:
    template <class Ch, class Tr>
    constexpr void print(std::basic_ostream<Ch, Tr>& os) const {
        using namespace detail;
        std::apply(
            [&os, this](const auto& arg,
                        const auto&... args) {
                _present(os, arg);
                ((os << _delim, _present(os, args)), ...);
            },
            _tuple);
    }

   private:
    const std::tuple<Args...>& _tuple;
    std::string_view _delim;
    my::make_member_function_t<Presenter> _present;
};

template <class Presenter = DefaultPresenter>
struct PairPresenter {
    constexpr explicit PairPresenter(std::string_view delim = ", ",
                                     Presenter presenter = {})
        : _delim(std::move(delim)), _present(presenter) {
    }

    template <pair_like T, class Ch, class Tr>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        _present(os, value.first);
        os << _delim;
        _present(os, value.second);
    }

   private:
    const std::string_view _delim;
    Presenter _present;
};

constexpr PairPresenter<> pairPresent;

template <class Presenter = DefaultPresenter>
struct RangePresenter {
   public:
    constexpr explicit RangePresenter(std::string_view delim = ", ",
                                      Presenter presenter = {})
        : _delim(std::move(delim)), _present(presenter) {
    }

    template <std::ranges::range Range, class Ch, class Tr>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const Range& value) const {
        auto first = std::ranges::begin(value);
        auto last = std::ranges::end(value);

        if (first == last) return;

        auto it = first;
        _present(os, *it);

        for (++it; it != last; ++it) {
            os << _delim;
            _present(os, *it);
        }
    }

   private:
    const std::string_view _delim;
    Presenter _present;
};

constexpr RangePresenter<> rangePresent;
template <class Presenter = DefaultPresenter>
struct TuplePresenter {
   public:
    constexpr explicit TuplePresenter(std::string_view delim = ", ",
                                      Presenter presenter = {})
        : _delim(std::move(delim)), _present(presenter) {
    }

    template <class Ch, class Tr, class... Args>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const std::tuple<Args...>& value) const {
        std::apply(
            [&os, this](const auto& arg,
                        const auto&... args) {
                _present(os, arg);
                ((os << _delim, _present(os, args)), ...);
            },
            value);
    }

   private:
    const std::string_view _delim;
    Presenter _present;
};

constexpr TuplePresenter<> tuplePresent;

struct JoinFunction {
    template <std::ranges::range Range, class Presenter = DefaultPresenter>
    constexpr auto operator()(const Range& range,
                              std::string_view delim = ", ",
                              Presenter presenter = {}) const {
        return JoinedRangeView(std::ranges::begin(range),
                               std::ranges::end(range),
                               delim, presenter);
    }

    template <class Presenter, class... Args>
    constexpr auto operator()(const std::tuple<Args...>& tuple,
                              std::string_view delim = ", ",
                              Presenter presenter = DefaultPresenter{}) const {
        return JoinedTupleView(tuple, delim, presenter);
    }
};

constexpr JoinFunction join;

template <class Ch, class Tr, default_presentable<Ch, Tr> T>
constexpr void present(std::basic_ostream<Ch, Tr>& os, const T& value) {
    if constexpr (my::printable<T>) {
        os << value;
    } else if constexpr (std::ranges::range<T>) {
        rangePresent(os, value);
    } else if constexpr (pair_like<T>) {
        pairPresent(os, value);
    } else if constexpr (my::is_tuple_v<T>) {
        tuplePresent(os, value);
    } else if constexpr (std::input_or_output_iterator<T>) {
        present(os, *value);
    }
    return;
}

}  // namespace my