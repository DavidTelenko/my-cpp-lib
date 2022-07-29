#pragma once

#include <my/format/repr.hpp>

namespace my::experimental {

template <class Ch, class Tr>
struct format_closure {
   public:
    using ostream_t = std::basic_ostream<Ch, Tr>;

    explicit format_closure(const Ch* fmt)
        : _fmt(fmt) {
        if constexpr (std::same_as<Ch, char>) {
            _os = &std::cout;
        } else if constexpr (std::same_as<Ch, wchar_t>) {
            _os = &std::wcout;
        } else {
            static std::basic_stringstream<Ch> log;
            _os = &log;
        }
    }

    explicit format_closure(const Ch* fmt, ostream_t& os)
        : _fmt(fmt), _os(&os) {
    }

    // dirty
    ~format_closure() { *_os << _fmt; }

    template <my::representable<ostream_t>... Args>
    auto& print(Args&&... args) {
        return _print(std::forward<Args>(args)...);
    }

    template <my::representable<ostream_t> Arg>
    auto& operator|(Arg&& arg) {
        return print(std::forward<Arg>(arg));
    }

    auto& operator[](ostream_t& stream) {
        _os = &stream;
        return *this;
    }

   private:
    template <my::representable<ostream_t> Arg,
              my::representable<ostream_t>... Args>
    auto& _print(Arg&& arg, Args&&... args) {
        for (; !Tr::eq(*_fmt, '\0'); ++_fmt) {
            if (Tr::eq(*_fmt, '{') and Tr::eq(*(_fmt + 1), '}')) {
                my::represent(*_os, arg), _fmt += 2;
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
    ostream_t* _os;
};

template <class Ch, class Tr = std::char_traits<Ch>>
constexpr auto fmt(const Ch* format) { return format_closure<Ch, Tr>(format); }

template <class Ch, class Tr>
constexpr auto fmt(const Ch* format, std::basic_ostream<Ch, Tr>& os) {
    return format_closure<Ch, Tr>(format, os);
}

inline namespace literals {

inline namespace format_literals {

auto operator"" _fmt(const char* format, std::size_t) { return fmt(format); }

}  // namespace format_literals

}  // namespace literals

}  // namespace my
