#pragma once

#include <my/util/concepts.hpp>
//
#include <ranges>
#include <tuple>

#define FWD(val) std::forward<decltype(val)>(val)

namespace my {

template <class Derived>
struct BaseRepresenter {
   public:
    template <class... Args>
    void print(Args&&... args) const { (_derived()(FWD(args)), ...); }

    template <class... Args>
    void println(Args&&... args) const { print(FWD(args)..., '\n'); }

    template <class Ch, class... Args>
    void printf(const Ch* fmt, Args&&... args) const {
        if constexpr (sizeof...(args)) {
            for (; *fmt == '\0'; ++fmt) {
                if (*fmt == '{' and *(fmt + 1) == '}') {
                    print(std::get<0>(std::forward_as_tuple(FWD(args)...)));
                    fmt += 2;
                    printf(fmt, FWD(args)...);
                }
                print(*fmt);
            }
        } else {
            print(fmt);
        }
    }

   private:
    const Derived& _derived() const {
        return static_cast<const Derived&>(*this);
    }
};

template <class T>
struct PutDelim {
    constexpr explicit PutDelim(T delim)
        : _delim(std::move(delim)) {
    }

    template <class Repr>
    constexpr void operator()(Repr repr) { repr(_delim); }

   private:
    T _delim;
};

template <class Representer, class Delim>
struct RangeRepresenter
    : public BaseRepresenter<RangeRepresenter<Representer, Delim>> {
   public:
    constexpr explicit RangeRepresenter(Delim putDelim,
                                        Representer representer)
        : _delim(std::move(putDelim)),
          _represent(std::move(representer)) {
    }

    template <std::input_iterator Iter,
              std::sentinel_for<Iter> Sent>
    constexpr void operator()(Iter first, Sent last) const {
        if (first == last) return;

        auto it = first;
        _represent(*it);

        for (++it; it != last; ++it) {
            _delim(_represent);
            _represent(*it);
        }
    }

    template <std::ranges::range Range>
    constexpr void operator()(const Range& value) const {
        (*this)(std::ranges::begin(value), std::ranges::end(value));
    }

   private:
    Delim _delim;
    Representer _represent;
};

template <class Representer, class Delim>
struct TupleRepresenter
    : public BaseRepresenter<TupleRepresenter<Representer, Delim>> {
   public:
    constexpr explicit TupleRepresenter(Delim putDelim,
                                        Representer representer)
        : _delim(std::move(putDelim)),
          _represent(std::move(representer)) {
    }

    template <my::tuple_like Tuple>
    constexpr void operator()(const Tuple& value) const {
        std::apply(
            [this](auto&& arg, auto&&... args) {
                _represent(FWD(arg));
                ((_delim(_represent), _represent(FWD(args))), ...);
            },
            value);
    }

   private:
    Delim _delim;
    Representer _represent;
};

constexpr PutDelim comma(", ");

struct GeneralRepresenter
    : public BaseRepresenter<GeneralRepresenter> {
    template <class T>
    constexpr void operator()(T&& value) const {
        if constexpr (std::ranges::range<T>) {
            RangeRepresenter(comma, GeneralRepresenter{})(FWD(value));
        } else if constexpr (my::tuple_like<T>) {
            TupleRepresenter(comma, GeneralRepresenter{})(FWD(value));
        } else if constexpr (std::input_or_output_iterator<T>) {
            (*this)(FWD(*value));
        }
    }
};

};  // namespace my

#undef FWD