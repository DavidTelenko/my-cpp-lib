#pragma once

#ifdef MY_LOG_COLORED
#include <my/format/color.hpp>
#define IF_MY_LOG_COLORED(expr) (expr)
#else
#define IF_MY_LOG_COLORED(...)
#endif

#include <my/format/format.hpp>
#include <my/format/repr.hpp>

namespace my::experimental {

#ifdef MY_LOG_COLORED
struct LogPrintColors {
    static my::Color ErrorColor;
    static my::Color WarnColor;
    static my::Color InfoColor;
};
my::Color LogPrintColors::ErrorColor = my::Color::fromHex(0xff0000);
my::Color LogPrintColors::WarnColor = my::Color::fromHex(0xffaa00);
my::Color LogPrintColors::InfoColor = my::Color::fromHex(0x70ff80);
#endif

template <class Ch, class Tr,
          my::representable<std::basic_ostream<Ch, Tr>>... Args>
auto log(std::basic_ostream<Ch, Tr>& os, const Ch* format, Args&&... args) {
    my::pretty.printf(os, format, std::forward<Args>(args)...);
    os << os.widen('\n');
}

template <class Ch,
          my::representable<std::basic_ostream<Ch>>... Args>
auto log(const Ch* format, Args&&... args) {
    if constexpr (std::same_as<Ch, wchar_t>) {
        log(std::wcout, format, std::forward<Args>(args)...);
    } else {
        log(std::cout, format, std::forward<Args>(args)...);
    }
}

template <class Ch,
          my::representable<std::basic_ostream<Ch>>... Args>
auto error(const Ch* format, Args&&... args) {
    IF_MY_LOG_COLORED(std::cerr << my::fg(LogPrintColors::ErrorColor));
    my::printf(std::cerr, "[Error]: ");
    my::log(std::cerr, format, std::forward<Args>(args)...);
    IF_MY_LOG_COLORED(std::cerr << my::resetcol);
}

template <class Ch,
          my::representable<std::basic_ostream<Ch>>... Args>
auto warn(const Ch* format, Args&&... args) {
    IF_MY_LOG_COLORED(std::cerr << my::fg(LogPrintColors::WarnColor));
    my::printf(std::cerr, "[Warn]: ");
    my::log(std::cerr, format, std::forward<Args>(args)...);
    IF_MY_LOG_COLORED(std::cerr << my::resetcol);
}

template <class Ch,
          my::representable<std::basic_ostream<Ch>>... Args>
auto info(const Ch* format, Args&&... args) {
    IF_MY_LOG_COLORED(std::cerr << my::fg(LogPrintColors::InfoColor));
    my::printf(std::cerr, "[Info]: ");
    my::log(std::cerr, format, std::forward<Args>(args)...);
    IF_MY_LOG_COLORED(std::cerr << my::resetcol);
}

}  // namespace my