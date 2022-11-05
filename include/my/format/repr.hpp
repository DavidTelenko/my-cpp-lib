#pragma once

#include <my/util/concepts.hpp>
#include <my/util/functional.hpp>
#include <my/util/utils.hpp>
//
#include <iomanip>
#include <iostream>
#include <iterator>
#include <ranges>
#include <sstream>
#include <string>
#include <variant>

namespace my {

namespace detail {

template <class Ch, class Tr, my::representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _represent(std::basic_ostream<Ch, Tr>& os, const T& value);

template <class Ch, class Tr, my::representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _prettyRepresent(std::basic_ostream<Ch, Tr>& os, const T& value);

}  // namespace detail

template <class Representer, class Ch, class Tr>
struct BaseRepresenterClosure {
   public:
    using ostream_t = std::basic_ostream<Ch, Tr>;

    constexpr explicit BaseRepresenterClosure(ostream_t& os, Representer repr)
        : _os(std::addressof(os)), _repr(std::move(repr)) {}

    auto& setStream(ostream_t& os) {
        _os = &os;
        return *this;
    }

    auto& operator[](ostream_t& stream) {
        return setStream(stream);
    }

    template <my::representable_with<Representer, ostream_t>... Args>
    auto& print(Args&&... args) {
        return _print(std::forward<Args>(args)...);
    }

    template <my::representable_with<Representer, ostream_t>... Args>
    auto& operator()(Args&&... args) {
        return _print(std::forward<Args>(args)...);
    }

    template <my::representable_with<Representer, ostream_t> Arg>
    auto& operator<<(Arg&& arg) {
        return _print(std::forward<Arg>(arg));
    }

   protected:
    template <my::representable_with<Representer, ostream_t>... Args>
    auto& _print(Args&&... args) {
        (_repr(*_os, args), ...);
        return *this;
    }

    ostream_t* _os;
    Representer _repr;
};

template <class Representer, my::representable_with<Representer> T>
class RepresentableValueView {
   public:
    constexpr explicit RepresentableValueView(Representer represent,
                                              const T& value)
        : _represent(std::move(represent)),
          _value(value) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const RepresentableValueView& obj) {
        obj._represent(os, obj._value);
        return os;
    }

   private:
    Representer _represent;
    const T& _value;
};

template <class T>
constexpr auto emitter(T&& val) {
    // clang-format off
    return [val = std::forward<T>(val)]<class Ch, class Tr>
        requires my::printable<T, std::basic_ostream<Ch, Tr>>
        (std::basic_ostream<Ch, Tr> & os) {
            os << val;
        };
    // clang-format on
}

inline namespace repr {

template <class Derived>
struct BaseRepresenter {
    template <class Ch, class Tr, my::representable_with<Derived> T>
    constexpr auto get(const T& value) const {
        std::basic_stringstream<Ch, Tr> ss;
        _derived()(ss, value);
        return ss.str();
    }

    template <my::representable_with<Derived> T>
    constexpr auto get(const T& value) const {
        return get<char, std::char_traits<char>>(value);
    }

    template <my::representable_with<Derived> T>
    constexpr auto view(const T& value) const {
        return RepresentableValueView(_derived(), value);
    }

   private:
    constexpr const auto& _derived() const noexcept {
        return static_cast<const Derived&>(*this);
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

template <class Representer, class PutDelim>
struct RangeRepresenter
    : public BaseRepresenter<RangeRepresenter<Representer, PutDelim>> {
   public:
    constexpr explicit RangeRepresenter(PutDelim putDelim,
                                        Representer representer)
        : _delim(std::move(putDelim)),
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
            _delim(os), _represent(os, *it);
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
        _delim(os), os << "...";

        if (!lastLength) {
            os << "(" << size - maxLength << ")";
            return;
        }

        if (size >= maxLength + lastLength) {
            os << "(" << size - maxLength - lastLength << ")";
            _delim(os);
            (*this)(os, std::ranges::next(first, size - lastLength), last);
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
    PutDelim _delim;
    Representer _represent;
};

template <class Representer, class PutDelim>
struct TupleRepresenter
    : public BaseRepresenter<TupleRepresenter<Representer, PutDelim>> {
   public:
    constexpr explicit TupleRepresenter(PutDelim putDelim,
                                        Representer representer)
        : _delim(std::move(putDelim)),
          _represent(std::move(representer)) {
    }

    template <class Ch, class Tr, class Tuple>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const Tuple& value) const {
        std::apply(
            [&os, this](const auto& arg, const auto&... args) {
                _represent(os, arg);
                ((_delim(os), _represent(os, args)), ...);
            },
            value);
    }

   private:
    PutDelim _delim;
    Representer _represent;
};

template <class Representer, class PutDelim>
using PairRepresenter =
    TupleRepresenter<Representer, PutDelim>;

constexpr RangeRepresenter rangeRepresent(emitter(", "), DefaultRepresenter{});
constexpr TupleRepresenter tupleRepresent(emitter(", "), DefaultRepresenter{});
constexpr PairRepresenter pairRepresent(emitter(", "), DefaultRepresenter{});

template <my::manipulator PutDelim,
          class Representer = DefaultRepresenter>
constexpr auto makeRangeRepresenter(
    PutDelim dlm = emitter(", "), Representer repr = {}) {
    return RangeRepresenter(dlm, repr);
}

template <class Delim = const char*,
          my::representer_for<Delim> Representer = DefaultRepresenter>
constexpr auto makeRangeRepresenter(
    Delim dlm = ", ", Representer repr = {}) {
    return RangeRepresenter(emitter(dlm), repr);
}

template <my::manipulator PutDelim,
          class Representer = DefaultRepresenter>
constexpr auto makeTupleRepresenter(
    PutDelim dlm = emitter(", "), Representer repr = {}) {
    return TupleRepresenter(dlm, repr);
}

template <class Delim = const char*,
          my::representer_for<Delim> Representer = DefaultRepresenter>
constexpr auto makeTupleRepresenter(
    Delim dlm = ", ", Representer repr = {}) {
    return TupleRepresenter(emitter(dlm), repr);
}

template <my::manipulator PutDelim,
          class Representer = DefaultRepresenter>
constexpr auto makePairRepresenter(
    PutDelim dlm = emitter(", "), Representer repr = {}) {
    return PairRepresenter(dlm, repr);
}

template <class Delim = const char*,
          my::representer_for<Delim> Representer = DefaultRepresenter>
constexpr auto makePairRepresenter(
    Delim dlm = ", ", Representer repr = {}) {
    return PairRepresenter(emitter(dlm), repr);
}

}  // namespace repr

struct PrettyOptions {
    static size_t rangeMaxLength;
    static size_t rangeMaxLengthFromEnd;
    static const char* rangeOpenDelim;
    static const char* rangeCloseDelim;
    static const char* tupleOpenDelim;
    static const char* tupleCloseDelim;
};

size_t PrettyOptions::rangeMaxLength = 10;
size_t PrettyOptions::rangeMaxLengthFromEnd = 10;
const char* PrettyOptions::rangeOpenDelim = "[";
const char* PrettyOptions::rangeCloseDelim = "]";
const char* PrettyOptions::tupleOpenDelim = "(";
const char* PrettyOptions::tupleCloseDelim = ")";

namespace detail {

template <class Ch, class Tr, my::representable<std::basic_ostream<Ch, Tr>> T>
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
constexpr void _applyCommonManips(std::basic_ostream<Ch, Tr>& os,
                                  const T& value) {
    using std::quoted;
    constexpr bool quotedApplicable = requires(T val) {
        {quoted(val)};
    };

    if constexpr (quotedApplicable) {
        os << quoted(value);
    } else if constexpr (std::same_as<T, Ch>) {
        os << '\'' << value << '\'';
    } else if constexpr (std::same_as<T, bool>) {
        os << (value ? "true" : "false");
    } else {
        os << value;
    }
}

constexpr TupleRepresenter _tuplePrettyRepresent(
    emitter(", "), PrettyRepresenter{});

constexpr RangeRepresenter _rangeOfValuesPrettyRepresent(
    emitter(", "), PrettyRepresenter{});

constexpr RangeRepresenter _rangeOfRangesRepresent(
    emitter(",\n"), PrettyRepresenter{});

template <class Ch, class Tr, my::representable<std::basic_ostream<Ch, Tr>> T>
constexpr void _prettyRepresent(std::basic_ostream<Ch, Tr>& os,
                                const T& value) {
    if constexpr (my::printable<T>) {
        _applyCommonManips(os, value);
    } else if constexpr (std::ranges::range<T>) {
        os << PrettyOptions::rangeOpenDelim;
        using value_t = std::ranges::range_value_t<T>;
        if constexpr (not my::printable<value_t> and
                      std::ranges::range<value_t>) {
            _rangeOfRangesRepresent(
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
        os << PrettyOptions::tupleCloseDelim;
    } else if constexpr (std::input_or_output_iterator<T>) {
        _prettyRepresent(os, *value);
    }
}

}  // namespace detail

inline namespace repr {

constexpr DefaultRepresenter represent;
constexpr PrettyRepresenter pretty;

}  // namespace repr

};  // namespace my