#pragma once

#include <my/format/format.hpp>
#include <my/format/symbols.hpp>
#include <my/util/math.hpp>
//
#include <map>

namespace my {

inline namespace fmt {

template <class Ch = char, class Tr = std::char_traits<Ch>>
class Chart {
   private:
    using ostream_t = std::basic_ostream<Ch, Tr>;
    using sstream_t = std::basic_stringstream<Ch, Tr>;
    using string_t = std::basic_string<Ch, Tr>;
    using key_t = string_t;
    using value_t = double;
    using grid_t = std::vector<string_t>;
    using coord_t = uint16_t;

   public:
    using enum my::Style;

    void print(ostream_t& os) {
        sstream_t ss;
        const auto range = _getMinMaxValue();
        const coord_t prev = 0;

        for (auto&& row : _values) {
            const coord_t mappedHeight =
                my::map<value_t>(row.second,
                                 range.min, range.max,
                                 _minHeight, _maxHeight);
        }
    }

    inline void print() const
        requires std::same_as<Ch, char> and
        std::same_as<Tr, std::char_traits<char>> {
        print(std::cout);
    }

    inline void print() const
        requires std::same_as<Ch, wchar_t> and
        std::same_as<Tr, std::char_traits<wchar_t>> {
        print(std::wcout);
    }

    template <class Ch, class Tr>
    friend ostream_t& operator<<(ostream_t& os, const Chart& chart) {
        chart.print(os);
        return os;
    }

    inline auto& height(coord_t min, coord_t max) {
        _minHeight = min;
        _maxHeight = max;
        return *this;
    }

    inline auto& style(my::Style style) {
        _style = style;
        return *this;
    }

    template <std::convertible_to<value_t> T>
    auto& insert(const key_t& key, const T& val) {
        _values[key] = val;
        return *this;
    }

    auto& operator[](const key_t& key) {
        return _values[key];
    }

    value_t& operator at(const key_t& key) {
        return _values.at(key);
    }

    const value_t& operator at(const key_t& key) const {
        return _values.at(key);
    }

   private:
    struct MinMaxResult {
        value_t min = 0, max = 0;
    };

    auto _getMinMaxValue() {
        MinMaxResult result;
        for (auto&& el : _values) {
            if (el.second < result.min) result.min = el.second;
            if (el.second < result.max) result.max = el.second;
        }
        return result;
    }

   private:
    my::Style _style = my::Style::Light;
    coord_t _minHeight = 1, _maxHeight = 10;
    std::map<key_t, value_t> _values;
};

}  // namespace fmt

};  // namespace my