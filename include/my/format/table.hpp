#pragma once

#include <my/format/repr.hpp>
#include <my/format/symbols.hpp>
#include <my/util/str_utils.hpp>
//
#include <cassert>
#include <ranges>
#include <vector>

namespace my {

template <class Representer = my::DefaultRepresenter,
          class Ch = char,
          class Tr = std::char_traits<Ch>>
struct Table {
    using enum my::Style;
    using string_t = std::basic_string<Ch, Tr>;
    using ostream_t = std::basic_ostream<Ch, Tr>;

   public:
    constexpr Table() = default;
    /**
     * @brief Adds row elements to table. If there is only
     * one parameter and it is iterable
     * elements will be read one by one,
     * else vararg specialization will be called thus each parameter
     * will be interpreted as single element in the table.
     *
     * @note if subsequent calls of this method will receive
     * more or less arguments then in first call, assertion will fail.
     *
     * @param arg first parameter if it is single iterable argument,
     * then iterable specialization will be called
     * @param args rest of arguments
     * @return auto& chain reference to table object
     */
    template <my::representable_with<Representer, ostream_t> Arg,
              my::representable_with<Representer, ostream_t>... Args>
    inline auto& pushRow(Arg&& arg, Args&&... args) {
        _body.push_back(_receiveRow(std::forward<Arg>(arg),
                                    std::forward<Args>(args)...));
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
    template <my::representable_with<Representer, ostream_t> Arg,
              my::representable_with<Representer, ostream_t>... Args>
    inline auto& header(Arg&& arg, Args&&... args) {
        _header = _receiveRow(std::forward<Arg>(arg),
                              std::forward<Args>(args)...);
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
    template <my::representable_with<Representer, ostream_t> Arg,
              my::representable_with<Representer, ostream_t>... Args>
    inline auto& footer(Arg&& arg, Args&&... args) {
        _footer = _receiveRow(std::forward<Arg>(arg),
                              std::forward<Args>(args)...);
        return *this;
    }

    /**
     * @brief Removes last added row from body.
     *
     * @return auto& chain reference to table object
     */
    inline auto& popRow() {
        _body.pop_back();
        return *this;
    }

    /**
     * @brief Attempt to preallocate enough memory for specified number of rows.
     *
     * @param n number of rows to preallocate
     * @return auto& chain reference to table object
     */
    inline auto& reserve(size_t n) {
        _body.reserve(n);
        return *this;
    }

    /**
     * @brief Retrieve reference to internal std::vector<std::vector<std::string>>
     * of string rows
     *
     * @return auto& to data
     */
    inline auto& data() { return _body; }

    /**
     * @brief Retrieve const reference to internal std::vector<std::vector<std::string>>
     * of string rows
     *
     * @return auto& to data
     */
    inline const auto& data() const { return _body; }

    /**
     * @brief Clear body of table
     *
     * @return auto& chain reference to table object
     */
    inline auto& clear() {
        _body.clear();
        return *this;
    }

    /**
     * @brief Range access
     *
     * @return respective iterator
     */
    inline auto begin() { return _body.begin(); }
    inline auto end() { return _body.end(); }
    inline auto begin() const { return _body.begin(); }
    inline auto end() const { return _body.end(); }

    /**
     * @brief Sets how much rows needs to be displayed after which
     * footer will appear.
     *
     * @param lines amount of rows required to omit printing footer.
     * @return auto& chain reference to table object
     */
    inline auto& footerAfter(size_t lines) {
        _footer_after_lines = lines;
        return *this;
    }

    /**
     * @brief Sets header to have the same value as footer.
     * Call one more time to switch
     *
     * @return auto& chain reference to table object
     */
    inline auto& sameHeaderFooter() {
        _same_header_footer = !_same_header_footer;
        return *this;
    }

    /**
     * @brief Sets weather to print separator after each row.
     * Call one more time to switch
     *
     * @return auto& chain reference to table object
     */
    inline auto& separateEach() {
        _separate_each = !_separate_each;
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
        _style = style;
        return *this;
    }

    /**
     * @brief Prints table into ostream. You can also use operator<<
     *
     * @param os ostream reference
     * @return ostream_t& reference to os
     */
    inline void print(ostream_t& os) const {
        if (_header.empty() and _body.empty() and _footer.empty()) return;
        _printHeader(os);
        _printBody(os);
        _printFooter(os);
    }

    /**
     * @brief Prints table into ostream using html table.
     *
     * @param os ostream reference
     * @return ostream_t& reference to os
     */
    inline void printHTML(ostream_t& os) const {
        if (_header.empty() and _body.empty() and _footer.empty()) return;
        _printHeaderHTML(os);
        _printBodyHTML(os);
        _printFooterHTML(os);
    }

    /**
     * @brief Prints table into std::cout. You can also use operator<<
     *
     * @return ostream_t& reference to std::cout
     */
    inline void print() const
        requires std::same_as<Ch, char> and
        std::same_as<Tr, std::char_traits<char>> {
        print(std::cout);
    }
    /**
     * @brief Prints table into std::wcout. You can also use operator<<
     *
     * @return ostream_t& reference to std::wcout
     */
    inline void print() const
        requires std::same_as<Ch, wchar_t> and
        std::same_as<Tr, std::char_traits<wchar_t>> {
        print(std::wcout);
    }

    friend ostream_t& operator<<(ostream_t& os, const Table& obj) {
        obj.print(os);
        return os;
    }

   private:
    // ASCII / UTF8 console printers

    inline auto _printHeader(ostream_t& os) const {
        _printFrontSeparator(os);

        if (_header.empty()) return;

        _printRowHelper(os, _header);
        if (not _body.empty() or
            not _footer.empty()) {
            _printSeparator(os);
        }
    }

    auto _printBody(ostream_t& os) const {
        if (_body.empty()) return;

        if (not _separate_each) {
            for (auto&& row : _body) {
                _printRowHelper(os, row);
            }
        } else {
            _printRowHelper(os, _body.front());
            for (auto&& row : _body | std::views::drop(1)) {
                _printSeparator(os);
                _printRowHelper(os, row);
            }
        }
    }

    auto _printFooter(ostream_t& os) const {
        if ((_footer_after_lines and _footer_after_lines >= _body.size()) or
            (_footer.empty() and not _same_header_footer)) {
            _printBackSeparator(os);
            return;
        }
        _printSeparator(os);
        _printRowHelper(os, _same_header_footer ? _header : _footer);
        _printBackSeparator(os);
    }

    inline auto _printFrontSeparator(ostream_t& os) const {
        assert(not _sizes.empty());
        //                       ─  ╭  ┬  ╮
        _printSeparatorHelper(os, 0, 2, 3, 4);
    }

    inline auto _printSeparator(ostream_t& os) const {
        //                       ─  ├  ┼  ┤
        _printSeparatorHelper(os, 0, 5, 6, 7);
    }

    inline auto _printBackSeparator(ostream_t& os) const {
        //                       ─  ╰  ┴  ╯
        _printSeparatorHelper(os, 0, 8, 9, 10);
    }

    // HTML printers
    auto _printHeaderHTML(ostream_t& os) const {
        os << "<table>";
        if (_header.empty()) return;

        _printRowHelperHTML(os, "th", _header);
    }

    auto _printBodyHTML(ostream_t& os) const {
        for (auto&& el : _body) {
            _printRowHelperHTML(os, "td", el);
        }
    }

    auto _printFooterHTML(ostream_t& os) const {
        if ((_footer_after_lines and _footer_after_lines >= _body.size()) or
            (_footer.empty() and not _same_header_footer)) {
            os << "</table>";
            return;
        }
        _printRowHelperHTML(os, "th", _same_header_footer ? _header : _footer);
        os << "</table>";
    }

    auto _printRowHelperHTML(ostream_t& os, const Ch* tag,
                             const std::vector<string_t>& row) const {
        os << "<tr>";
        for (auto&& el : row) {
            os << "<" << tag << ">" << el << "</" << tag << ">";
        }
        os << "</tr>";
    }

    // helpers

    inline auto
    _printRowHelper(ostream_t& os, const std::vector<string_t>& row) const {
        const auto dash = my::styles[static_cast<size_t>(_style)][1];  // │
        auto row_iter = row.begin();
        auto size_iter = _sizes.begin();
        auto row_end = row.end();
        auto size_end = _sizes.end();
        for (; row_iter != row_end and size_iter != size_end;
             ++row_iter, ++size_iter) {
            const auto element_size = row_iter->size();
            const auto size = *size_iter;

            os << dash << ' ' << *row_iter << ' ';

            if (element_size < size) {
                print_n(os, size - element_size, ' ');
            }
        }

        os << dash << '\n';
    }

    auto _printSeparatorHelper(ostream_t& os,
                               size_t main, size_t corner1,
                               size_t inter, size_t corner2) const {
        const auto dash =
            my::styles[static_cast<size_t>(_style)][main];
        const auto left_corner =
            my::styles[static_cast<size_t>(_style)][corner1];
        const auto t_shape =
            my::styles[static_cast<size_t>(_style)][inter];
        const auto right_corner =
            my::styles[static_cast<size_t>(_style)][corner2];

        os << left_corner;
        print_n(os, _sizes.front() + _pad, dash);

        for (auto size : _sizes | std::views::drop(1)) {
            os << t_shape;
            print_n(os, size + _pad, dash);
        }

        os << right_corner << '\n';
    }

    template <std::input_iterator It>
    auto _readRow(It begin, It end) {
        const size_t size = std::ranges::distance(begin, end);

        assert(size);
        if (_sizes.empty()) _sizes.resize(size);

        std::vector<string_t> row;
        row.reserve(size);

        for (auto size_iter = _sizes.begin(),
                  size_end = _sizes.end();
             begin != end and size_iter != size_end;
             ++begin, ++size_iter) {
            const auto value = _represent.template get<Ch, Tr>(*begin);
            const size_t value_size = value.size();

            if (value_size > *size_iter) {
                *size_iter = value_size;
            }

            row.push_back(std::move(value));
        }

        return row;
    }

    template <class It,
              my::representable_with<Representer, ostream_t> Arg,
              my::representable_with<Representer, ostream_t>... Args>
    auto _readVariadicRowImpl(std::vector<string_t>& row, It size_iter,
                              Arg&& arg, Args&&... args) {
        const auto value = _represent.template get<Ch, Tr>(arg);
        const size_t value_size = value.size();

        if (value_size > *size_iter) {
            *size_iter = value_size;
        }

        row.push_back(std::move(value));

        if constexpr (sizeof...(args) == 0) {
            return;
        } else {
            _readVariadicRowImpl(row, size_iter + 1,
                                 std::forward<Args>(args)...);
        }
    }

    template <my::representable_with<Representer, ostream_t>... Args>
    auto _readVariadicRow(Args&&... args) {
        constexpr size_t size = sizeof...(args);

        if (_sizes.empty()) _sizes.resize(size);
        assert(_sizes.size() == size);

        std::vector<string_t> row;
        row.reserve(size);

        auto size_iter = _sizes.begin();

        _readVariadicRowImpl(row, size_iter, std::forward<Args>(args)...);

        return row;
    }

    template <my::representable_with<Representer, ostream_t> Arg,
              my::representable_with<Representer, ostream_t>... Args>
    inline auto _receiveRow(Arg&& arg, Args&&... args) {
        if constexpr (sizeof...(args) == 0 and std::ranges::range<Arg>) {
            return _readRow(std::ranges::begin(arg), std::ranges::end(arg));
        } else {
            return _readVariadicRow(std::forward<Arg>(arg),
                                    std::forward<Args>(args)...);
        }
    }

    template <class T>
    constexpr static void
    print_n(ostream_t& os, size_t n, T&& val) {
        std::ranges::fill_n(std::ostream_iterator<T, Ch, Tr>(os),
                            n, std::forward<T>(val));
    }

    std::vector<string_t> _header;
    std::vector<std::vector<string_t>> _body;
    std::vector<string_t> _footer;
    std::vector<size_t> _sizes;
    //
    my::Style _style = my::Style::Curvy;
    bool _same_header_footer = false;
    bool _separate_each = false;
    size_t _footer_after_lines = 0;
    const uint16_t _pad = 2;

    Representer _represent;
};

namespace detail {

template <std::ranges::range T>
auto _tableIterable(const T& iterable) {
    Table t;
    t.reserve(std::ranges::size(iterable));

    for (auto&& el : iterable) {
        t.pushRow(el);
    }

    return t;
}

template <std::ranges::range T>
auto _tableMap(const T& map) {
    Table t;
    t.reserve(std::ranges::size(map));

    for (auto&& el : map) {
        t.pushRow(el.first, el.second);
    }

    return t;
}

template <std::ranges::range T, class... Projections>
auto _tableObjects(const T& objects, Projections... proj) {
    Table t;
    t.reserve(std::ranges::size(objects));

    for (auto&& el : objects) {
        t.pushRow(std::invoke(proj, el)...);
    }

    return t;
}

}  // namespace detail

/**
 * @brief Creates ASCII table from std::range, if value is an associative container
 * table will be formed as 2xN table with key value pairs in each row, otherwise
 * if range itself contains ranges each range element will occupy each element in row,
 * else if inside of range there is just printable values table will be formed as
 * 1xN table with each element in single row
 *
 *
 * @tparam T std::range
 * @param val value to represent as table
 * @return printable Table object
 */
template <std::ranges::range T>
constexpr auto table(const T& val) {
    if constexpr (my::pair_like<std::ranges::range_value_t<T>>) {
        return detail::_tableMap(val);
    } else {
        return detail::_tableIterable(val);
    }
}

/**
 * @brief Creates ASCII table from iterable of objects, provide projections
 * for each field of structure to represent it in row as string, each return value)
 * of projection must be printable.
 *
 * @tparam T std::range
 * @tparam Projections functions or projections that can be
 * invocable with expression std::invoke(proj, object
 * @param val range of objects
 * @param proj functions to project each value
 * @return printable Table object
 */
template <std::ranges::range T, class... Projections>
constexpr auto table(const T& val, Projections... proj) {
    return detail::_tableObjects(val, std::move(proj)...);
}

}  // namespace my