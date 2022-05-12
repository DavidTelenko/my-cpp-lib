#pragma once
#ifndef MY_TABLE_FORMAT_HPP
#define MY_TABLE_FORMAT_HPP

#include <my/format/join.hpp>
#include <my/format/symbols.hpp>
#include <my/util/str_utils.hpp>
#include <ranges>
#include <vector>

namespace my {

template <class Ch = char, class Tr = std::char_traits<Ch>>
struct Table {
    using enum my::Style;
    using string_t = std::basic_string<Ch, Tr>;
    using ostream_t = std::basic_ostream<Ch, Tr>;

   public:
    inline constexpr Table() = default;
    /**
     * @brief Adds row elements to table. If there is only
     * one parameter and it is iterable
     * elements will be read one by one,
     * else vararg specialization will be called thus each parameter
     * will be interpreted as single element in the table.
     *
     * @note if subsequent calls of this method will receive
     * more or less arguments then in first call assertion will fail.
     *
     * @param arg first parameter if it is single iterable argument,
     * then iterable specialization will be called
     * @param args rest of arguments
     * @return auto& chain reference to table object
     */
    template <my::joinable<Ch, Tr> Arg, my::joinable<Ch, Tr>... Args>
    inline auto& pushRow(const Arg& arg, const Args&... args) {
        body_.push_back(receiveRow_(arg, args...));
        return *this;
    }

    /**
     * @brief Sets header for columns. Works exactly the same
     * way as pushRow() does.
     * @see puhRow()
     *
     * @param arg first parameter if it is single iterable argument,
     * then iterable specialization will be called
     * @param args rest of arguments
     * @return auto& chain reference to table object
     */
    template <my::joinable<Ch, Tr> Arg, my::joinable<Ch, Tr>... Args>
    inline auto& header(const Arg& arg, const Args&... args) {
        header_ = receiveRow_(arg, args...);
        return *this;
    }

    /**
     * @brief Sets footer for columns. Works exactly the same
     * way as pushRow() does.
     * @see puhRow()
     *
     * @param arg first parameter if it is single iterable argument,
     * then iterable specialization will be called
     * @param args rest of arguments
     * @return auto& chain reference to table object
     */
    template <my::joinable<Ch, Tr> Arg, my::joinable<Ch, Tr>... Args>
    inline auto& footer(const Arg& arg, const Args&... args) {
        footer_ = receiveRow_(arg, args...);
        return *this;
    }

    /**
     * @brief Removes last added row from body.
     *
     * @return auto& chain reference to table object
     */
    inline auto& popRow() {
        body_.pop_back();
        return *this;
    }

    /**
     * @brief Attempt to preallocate enough memory for specified number of rows.
     *
     * @param n number of rows to preallocate
     * @return auto& chain reference to table object
     */
    inline auto& reserve(size_t n) {
        body_.reserve(n);
        return *this;
    }

    /**
     * @brief Sets how much rows needs to be displayed after which
     * footer will appear.
     *
     * @param lines amount of rows required to omit printing footer.
     * @return auto& chain reference to table object
     */
    inline auto& footerAfter(size_t lines) {
        footer_after_lines_ = lines;
        return *this;
    }

    /**
     * @brief Sets header to have the same value as footer.
     * Call one more time to switch
     *
     * @return auto& chain reference to table object
     */
    inline auto& sameHeaderFooter() {
        same_header_footer_ = !same_header_footer_;
        return *this;
    }

    /**
     * @brief Sets weather to print separator after each row.
     * Call one more time to switch
     *
     * @return auto& chain reference to table object
     */
    inline auto& separateEach() {
        separate_each_ = !separate_each_;
        return *this;
    }

    /**
     * @brief Sets style of table.
     * @note Table uses enum my::Style so you can get style either from
     * enum or class object.
     *
     * @param style my::Style enum value
     * @return auto& chain reference to table object
     */
    inline auto& style(my::Style style) {
        style_ = style;
        return *this;
    }

    /**
     * @brief Prints table into ostream. You can also use operator<<
     *
     * @param os ostream reference
     * @return ostream_t& reference to os
     */
    inline ostream_t& print(ostream_t& os) const {
        if (header_.empty() and body_.empty() and footer_.empty())
            return os;
        printHeader(os);
        printBody(os);
        printFooter(os);
        return os;
    }

    /**
     * @brief Prints table into std::cout. You can also use operator<<
     *
     * @return ostream_t& reference to std::cout
     */
    inline ostream_t& print() const
        requires std::same_as<Ch, char> and
        std::same_as<Tr, std::char_traits<char>> {
        return print(std::cout);
    }
    /**
     * @brief Prints table into std::wcout. You can also use operator<<
     *
     * @return ostream_t& reference to std::wcout
     */
    inline ostream_t& print() const
        requires std::same_as<Ch, wchar_t> and
        std::same_as<Tr, std::char_traits<wchar_t>> {
        return print(std::wcout);
    }

    friend ostream_t& operator<<(ostream_t& os, const Table& obj) {
        return obj.print(os);
    }

   private:
    inline auto printHeader(ostream_t& os) const {
        printFrontSeparator(os);

        if (header_.empty()) return;

        printRowHelper(os, header_);
        printSeparator(os);

        if (body_.empty()) {
            printBackSeparator(os);
            return;
        }
    }

    auto printBody(ostream_t& os) const {
        if (not separate_each_) {
            for (auto&& row : body_) {
                printRowHelper(os, row);
            }
        } else {
            printRowHelper(os, body_.front());
            for (auto&& row : body_ | std::views::drop(1)) {
                printSeparator(os);
                printRowHelper(os, row);
            }
        }
    }

    auto printFooter(ostream_t& os) const {
        if ((footer_after_lines_ and footer_after_lines_ >= body_.size()) or
            (footer_.empty() and not same_header_footer_)) {
            printBackSeparator(os);
            return;
        }
        printSeparator(os);
        printRowHelper(os, same_header_footer_ ? header_ : footer_);
        printBackSeparator(os);
    }

    inline auto printFrontSeparator(ostream_t& os) const {
        assert(not sizes_.empty());
        //                       ─  ╭  ┬  ╮
        printSeparatorHelper(os, 0, 2, 3, 4);
    }

    inline auto printSeparator(ostream_t& os) const {
        //                       ─  ├  ┼  ┤
        printSeparatorHelper(os, 0, 5, 6, 7);
    }

    inline auto printBackSeparator(ostream_t& os) const {
        //                       ─  ╰  ┴  ╯
        printSeparatorHelper(os, 0, 8, 9, 10);
    }

    // helpers

    inline auto
    printRowHelper(ostream_t& os, const std::vector<string_t>& row) const {
        const auto dash = my::styles[static_cast<size_t>(style_)][1];  // │

        for (auto row_iter = begin(row), size_iter = begin(sizes_),
                  row_end = end(row), size_end = end(sizes_);
             row_iter != row_end and size_iter != size_end;
             ++row_iter, ++size_iter) {
            const auto element_size = row_iter->size();
            const auto size = *size_iter;

            os << dash << my::space << *row_iter << my::space;

            if (element_size < size) {
                print_n(os, size - element_size, os.widen(' '));
            }
        }

        os << dash << my::newline;
    }

    auto printSeparatorHelper(ostream_t& os,
                              size_t main, size_t corner1,
                              size_t inter, size_t corner2) const {
        const auto dash =
            my::styles[static_cast<size_t>(style_)][main];
        const auto left_corner =
            my::styles[static_cast<size_t>(style_)][corner1];
        const auto t_shape =
            my::styles[static_cast<size_t>(style_)][inter];
        const auto right_corner =
            my::styles[static_cast<size_t>(style_)][corner2];

        os << left_corner;
        print_n(os, sizes_.front() + pad, dash);

        for (auto size : sizes_ | std::views::drop(1)) {
            os << t_shape;
            print_n(os, size + pad, dash);
        }

        os << right_corner << my::newline;
    }

    template <std::input_iterator It>
    auto readRow_(It begin, It end) {
        const size_t size = std::distance(begin, end);

        assert(size);
        if (sizes_.empty()) sizes_.resize(size);

        std::vector<string_t> row;
        row.reserve(size);

        for (auto size_iter = sizes_.begin(),
                  size_end = sizes_.end();
             begin != end and size_iter != size_end;
             ++begin, ++size_iter) {
            const auto value = my::toString<Ch, Tr>(my::join<Ch>(*begin));
            const size_t value_size = value.size();

            if (value_size > *size_iter) {
                *size_iter = value_size;
            }

            row.push_back(std::move(value));
        }

        return row;
    }

    template <class It, my::joinable<Ch, Tr> Arg,
              my::joinable<Ch, Tr>... Args>
    auto readVariadicRowImpl_(std::vector<string_t>& row, It size_iter,
                              const Arg& arg, const Args&... args) {
        const auto value = my::toString<Ch, Tr>(my::join<Ch, Tr>(arg));
        const size_t value_size = value.size();

        if (value_size > *size_iter) {
            *size_iter = value_size;
        }

        row.push_back(std::move(value));

        if constexpr (sizeof...(args) == 0) {
            return;
        } else {
            readVariadicRowImpl_(row, size_iter + 1, args...);
        }
    }

    template <my::joinable<Ch, Tr>... Args>
    auto readVariadicRow_(const Args&... args) {
        constexpr size_t size = sizeof...(args);

        if (sizes_.empty()) sizes_.resize(size);
        assert(sizes_.size() == size);

        std::vector<string_t> row;
        row.reserve(size);

        auto size_iter = sizes_.begin();

        readVariadicRowImpl_(row, size_iter, args...);

        return row;
    }

    template <my::joinable<Ch, Tr> Arg, my::joinable<Ch, Tr>... Args>
    inline auto receiveRow_(const Arg& arg, const Args&... args) {
        if constexpr (sizeof...(args) == 0 and my::is_iterable_v<Arg>) {
            using std::begin;
            using std::end;
            return readRow_(begin(arg), end(arg));
        } else {
            return readVariadicRow_(arg, args...);
        }
    }

    template <class T>
    inline constexpr static void
    print_n(ostream_t& os, size_t n, T&& val) {
        std::ranges::fill_n(std::ostream_iterator<T, Ch, Tr>(os),
                            n, std::forward<T>(val));
    }

    std::vector<string_t> header_;
    std::vector<std::vector<string_t>> body_;
    std::vector<string_t> footer_;
    std::vector<size_t> sizes_;
    std::vector<string_t> current_row_;
    //
    my::Style style_ = my::Style::Curvy;
    bool same_header_footer_ = false;
    bool separate_each_ = false;
    size_t footer_after_lines_ = 0;
    const uint16_t pad = 2;
};

template <my::iterable T>
auto tableIterable(const T& iterable) {
    using std::size;

    Table t;
    t.reserve(size(iterable));
    for (auto&& el : iterable) {
        t.pushRow(el);
    }

    return t;
}

template <my::associative_container T>
auto tableMap(const T& map) {
    using std::size;

    Table t;
    t.reserve(size(map));
    for (auto&& el : map) {
        t.pushRow(el.first, el.second);
    }

    return t;
}

template <my::iterable T, class... Projections>
auto tableObjects(const T& objects, Projections... proj) {
    using std::size;

    Table t;
    t.reserve(size(objects));
    for (auto&& el : objects) {
        t.pushRow(std::invoke(proj, el)...);
    }

    return t;
}

template <my::iterable T>
inline constexpr auto table(const T& val) {
    if constexpr (my::is_iterable_v<T> and
                  not my::is_associative_container_v<T>) {
        return tableIterable(val);
    } else if (my::is_associative_container_v<T>) {
        return tableMap(val);
    }
}

template <my::iterable T, class... Projections>
inline constexpr auto table(const T& val, Projections... proj) {
    return tableObjects(val, std::move(proj)...);
}

}  // namespace my
#endif  // MY_TABLE_FORMAT_HPP