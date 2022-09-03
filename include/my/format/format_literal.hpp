#pragma once

#include <my/format/repr.hpp>

namespace my {

template <class Ch, class Tr, class Representer>
struct format_closure {
   public:
    using ostream_t = std::basic_ostream<Ch, Tr>;

    explicit format_closure(ostream_t& os, const Ch* fmt, Representer repr)
        : _fmt(fmt), _initial(fmt), _repr(repr), _os(&os) {
    }

    // dirty
    ~format_closure() { *_os << _fmt; }

    template <my::representable_with<Representer, ostream_t>... Args>
    auto& print(Args&&... args) {
        return _print(std::forward<Args>(args)...);
    }

    template <my::representable_with<Representer, ostream_t>... Args>
    auto& operator()(Args&&... args) {
        return _print(std::forward<Args>(args)...);
    }

    template <my::representable_with<Representer, ostream_t> Arg>
    auto& operator|(Arg&& arg) {
        return _print(std::forward<Arg>(arg));
    }

    template <my::representable_with<Representer, ostream_t> Arg>
    auto& operator<<(Arg&& arg) {
        return _print(std::forward<Arg>(arg));
    }

    auto& setStream(ostream_t& stream) {
        _os = &stream;
        return *this;
    }

    auto& operator[](ostream_t& stream) {
        return setStream(stream);
    }

    auto& setFormat(const char* fmt) {
        _initial = fmt;
        _fmt = fmt;
        return *this;
    }

    auto& operator[](const char* format) {
        return setFormat(format);
    }

    [[deprecated("Experimental")]] auto& operator[](int) {
        return resetFormat();
    }

    auto& resetFormat() {
        _fmt = _initial;
        return *this;
    }

   private:
    template <my::representable_with<Representer, ostream_t> Arg,
              my::representable_with<Representer, ostream_t>... Args>
    auto& _print(Arg&& arg, Args&&... args) {
        for (; !Tr::eq(*_fmt, '\0'); ++_fmt) {
            if (Tr::eq(*_fmt, '{') and Tr::eq(*(_fmt + 1), '}')) {
                _repr(*_os, arg), _fmt += 2;
                return _print(std::forward<Args>(args)...);
            }
            *_os << *_fmt;
        }
        return *this;
    }

    auto& _print() {
        for (; !Tr::eq(*_fmt, '{') and
               !Tr::eq(*_fmt, '\0');
             ++_fmt) {
            *_os << *_fmt;
        }
        return *this;
    }

    const Ch* _fmt;
    const Ch* _initial;

    Representer _repr;
    ostream_t* _os;
};

template <class Ch, class Tr, class Representer>
constexpr auto fmt(std::basic_ostream<Ch, Tr>& os,
                   const Ch* format,
                   Representer repr) {
    return format_closure(os, format, std::move(repr));
}

template <class Ch, class Tr, class Representer>
constexpr auto fmt(const Ch* format,
                   Representer repr) {
    if constexpr (std::same_as<Ch, wchar_t>) {
        return fmt(std::wcout, format, std::move(repr));
    } else {
        return fmt(std::cout, format, std::move(repr));
    }
}

template <class Ch, class Representer>
constexpr auto fmt(const Ch* format,
                   Representer repr) {
    return fmt<Ch, std::char_traits<Ch>>(format, std::move(repr));
}

template <class Ch, class Tr>
constexpr auto fmt(std::basic_ostream<Ch, Tr>& os,
                   const Ch* format) {
    return fmt(os, format, my::represent);
}

template <class Ch>
constexpr auto fmt(const Ch* format) { return fmt(format, my::represent); }

inline namespace literals {

inline namespace format_literals {

auto operator"" _fmt(const char* format, std::size_t) { return fmt(format); }

}  // namespace format_literals

}  // namespace literals

}  // namespace my
