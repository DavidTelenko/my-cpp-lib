#pragma once

#include <my/format/repr.hpp>

namespace my {

template <class Ch, class Tr>
struct FormatEmitter {
    explicit FormatEmitter(const Ch* fmt)
        : _fmt(fmt) {
    }

    constexpr auto tail(std::basic_ostream<Ch, Tr>& os) const {
        for (; !Tr::eq(*_fmt, '\0'); ++_fmt) {
            if (Tr::eq(*_fmt, '{') and
                Tr::eq(*(_fmt + 1), '}')) {
                return true;
            }
            os << *_fmt;
        }
        return false;
    }

    constexpr auto operator()(std::basic_ostream<Ch, Tr>& os) const {
        if (tail(os)) {
            _fmt += 2;
            return true;
        }
        return false;
    }

   private:
    using str = const Ch*;
    mutable str _fmt;
};

template <class Representer, class Emitter>
struct FormatRepresenter
    : public BaseRepresenter<FormatRepresenter<Representer, Emitter>> {
   public:
    constexpr explicit FormatRepresenter(Emitter emitter,
                                         Representer representer)
        : _emitter(std::move(emitter)),
          _represent(std::move(representer)) {
    }

    template <class Ch, class Tr,
              my::representable_with<Representer, std::basic_ostream<Ch, Tr>> T>
    constexpr void operator()(std::basic_ostream<Ch, Tr>& os,
                              const T& value) const {
        if (_emitter(os)) _represent(os, value);
        _emitter.tail(os);
    }

   private:
    Emitter _emitter;
    Representer _represent;
};

template <class Representer, class Ch, class Tr>
using DefaultFormatRepresenter =
    FormatRepresenter<Representer, FormatEmitter<Ch, Tr>>;

template <class Representer, class Ch, class Tr>
using FormatClosure =
    my::BaseRepresenterClosure<
        DefaultFormatRepresenter<Representer, Ch, Tr>,
        Ch, Tr>;

template <class Ch, class Tr, class Representer>
constexpr auto fmt(std::basic_ostream<Ch, Tr>& os,
                   const Ch* format,
                   Representer repr) {
    return FormatClosure<Representer, Ch, Tr>(
        os,
        DefaultFormatRepresenter(
            FormatEmitter<Ch, Tr>(format),
            std::move(repr)));
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
