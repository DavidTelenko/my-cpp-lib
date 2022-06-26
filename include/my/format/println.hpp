#pragma once
#ifndef MY_PRINTLN_FORMAT_HPP
#define MY_PRINTLN_FORMAT_HPP

#include <my/format/join.hpp>
#include <my/format/manipulator_tag.hpp>
//
#include <cassert>
#include <locale>

namespace my {

template <class Ch, class Tr = std::char_traits<Ch>>
struct printer_base {
    constexpr explicit printer_base(std::basic_ostream<Ch, Tr>& os)
        : os_(os) {
    }

    inline auto& get() { return os_; }

   protected:
    std::basic_ostream<Ch, Tr>& os_;
};

/**
 * @brief Functional struct for print_t
 * produces comma separated joined output with newline at the end
 *
 * @tparam Ostream preferably std::basic_ostream
 */
template <class Ch, class Tr = std::char_traits<Ch>>
struct newline_printer : public printer_base<Ch, Tr> {
    constexpr explicit newline_printer(std::basic_ostream<Ch, Tr>& os)
        : printer_base<Ch, Tr>(os) {
    }

    template <my::joinable<Ch, Tr> Arg, my::joinable<Ch, Tr>... Args>
    auto& operator()(Arg&& val, Args&&... vals) {
        this->os_ << join<Ch, Tr>(std::forward<Arg>(val));
        ((this->os_ << comma << join<Ch, Tr>(std::forward<Args>(vals))), ...);
        this->os_ << newline;
        return *this;
    }

    auto& operator()() {
        this->os_ << newline;
        return *this;
    }

    template <my::joinable<Ch, Tr> Arg>
    friend auto& operator<<(newline_printer& p, Arg&& val) {
        return p(std::forward<Arg>(val));
    }
};

/**
 * @brief Functional struct for print_t
 * produces concatenated output without any separation
 *
 * @tparam Ostream preferably std::basic_ostream
 */
template <class Ch, class Tr = std::char_traits<Ch>>
struct push_printer : public printer_base<Ch, Tr> {
    constexpr explicit push_printer(std::basic_ostream<Ch, Tr>& os)
        : printer_base<Ch, Tr>(os) {
    }

    template <my::joinable<Ch, Tr>... Args>
    auto& operator()(Args&&... vals) {
        ((this->os_ << join<Ch, Tr>(std::forward<Args>(vals))), ...);
        return *this;
    }

    template <my::joinable<Ch, Tr> Arg>
    friend auto& operator<<(push_printer& p, Arg&& val) {
        return p(std::forward<Arg>(val));
    }
};

/**
 * @brief Manipulator for formatted_printer applies operator<<
 * to set format of underlying printer in print_t
 *
 * @tparam Char character of format string
 */
template <class Char = char>
struct setFormat {
    constexpr explicit setFormat(const Char* format)
        : format_(format) {
    }
    const Char* format_;
};

/**
 * @brief Convenience function for geenrating setFormat manipulator
 *
 * @param format
 * @return constexpr auto
 */
template <class Ch>
constexpr auto fmt(const Ch* format) noexcept {
    return setFormat(format);
}

/**
 * @brief Functional struct for print_t
 * produces formatted output by subsequent calls of operator<<, uses {} as anchor
 *
 * @see my::alt::printf
 *
 */
template <class Ch, class Tr = std::char_traits<Ch>>
struct formatted_printer : public printer_base<Ch, Tr> {
    constexpr explicit formatted_printer(std::basic_ostream<Ch, Tr>& os) noexcept
        : printer_base<Ch, Tr>(os) {
    }

    constexpr explicit formatted_printer(std::basic_ostream<Ch, Tr>& os,
                                         const Ch* format) noexcept
        : printer_base<Ch, Tr>(os), format_(format) {
    }

    template <my::joinable<Ch, Tr>... Args>
    inline auto& operator()(Args&&... args) {
        (dispatch_(std::forward<Args>(args)), ...);
        return *this;
    }

    template <my::joinable<Ch, Tr> Arg>
    inline friend auto& operator<<(formatted_printer& p, Arg&& arg) {
        return p(std::forward<Arg>(arg));
    }

    auto& format() { return format_; }
    const auto& format() const { return format_; }

   private:
    template <my::joinable<Ch, Tr> Arg>
    void dispatch_(Arg&& arg) {
        if constexpr (std::is_same_v<Arg, setFormat<Ch>>) {
            format_ = arg.format_;
        } else {
            printf_(std::forward<Arg>(arg));
        }
    }

    template <my::joinable<Ch, Tr> Arg>
    void printf_(Arg&& arg) {
        bool printed = false;

        for (;; ++format_) {
            if (*format_ == '{') {
                if (*(format_ + 1) != '}') {
                    this->os_ << *format_;
                    continue;
                }
                if (printed) break;

                this->os_ << join<Ch, Tr>(arg);

                format_ += 2;
                printed = true;
            }
            if (*format_ == '\0') break;

            this->os_ << *format_;
        }

        assert(printed and "Incorrect format: too many arguments for given format");
    }

    const Ch* format_{};
};

/**
 * @brief function wrapper of print_t object into std::cout
 * has two operators:
 *
 *  operator<< - yields similar behaviour to std::cout << val,
 *      except it prints newline after each subsequent call,
 *      if value was iterable, tuple, pair, iterator,
 *      uses join_proxy to provide operator<< for the object
 *
 *  operator() - is vararg function that will print paramenters
 *      in the following manor <arg1, arg2, arg3\\n>
 * @see my::join
 */
auto println = newline_printer(std::cout);

/**
 * @brief function wrapper of print_t object into std::wcout
 * @see println
 */
auto wprintln = newline_printer(std::wcout);

// alternative printf/println functions

template <class Ch, class Tr>
constexpr auto makePrintln(std::basic_ostream<Ch, Tr>& os) noexcept {
    return newline_printer<Ch, Tr>(os);
}

namespace alt {

/**
 * @brief Convenience wrapper for formatted_output printer. Can be used as follows:
 * printf(format_string) << first << second;
 * or
 * printf(format_string)(first, second);
 * @note if there is too many arguments for given format assertion will fail
 *       if format contains too many anchors only given amount of arguments will 
 *       be printed with prefix format until next anchor
 *       e.g printf("{}, {}, magic {} 42!") << "one" << "two";
 *           will produce the following output "one, two, magic ".
 *
 * @param format format string_view with replacement anchors as {}
 * @return inner print functor proxy
 */
auto printf = formatted_printer(std::cout);

/**
 * @brief function wrapper of print_t object into std::wcout
 * @see printf
 */
auto wprintf = formatted_printer(std::wcout);

template <class Ch, class Tr>
constexpr auto make_printf(std::basic_ostream<Ch, Tr>& os,
                           std::basic_string_view<Ch, Tr> format) noexcept {
    return formatted_printer<Ch, Tr>(os, format.data());
}

}  // namespace alt

}  // namespace my

#endif  // MY_PRINTLN_FORMAT_HPP