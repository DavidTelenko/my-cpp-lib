#pragma once

#include <my/format/symbols.hpp>
#include <my/util/math.hpp>
#include <my/util/str_utils.hpp>
#include <my/util/traits.hpp>
//
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace my {

inline namespace fmt {

template <class Number = float>
struct NumericRange {
    Number min, max, step;
}

struct PlotDimension {
    size_t width = 50, height = 20;
};

struct PlotPoint {
    PlotPoint(float x, float y) : x(x), y(y) {}
    PlotPoint() : x(0), y(0) {}
    friend auto& operator<<(std::ostream& os, const PlotPoint& p) {
        os << "(x:" << p.x << "; y:" << p.y << ")";
        return os;
    }
    float x, y;
};

namespace detail {

struct MinMaxXY {
    PlotPoint min, max;
    size_t maxLen;
};

template <class Iter>
auto getMinMaxXY(Iter b, Iter e) {
    MinMaxXY result;

    if constexpr (std::is_same_v<
                      typename std::iterator_traits<Iter>::value_type,
                      PlotPoint>) {
        PlotPoint min, max;
        size_t maxLen = 0;

        for (; b != e; ++b) {
            if (b->x < min.x) min.x = b->x;
            if (b->x > max.x) max.x = b->x;
            if (b->y < min.y) min.y = b->y;
            if (b->y > max.y) max.y = b->y;
            const auto len = strLength(b->y);
            if (maxLen < len) maxLen = len;
        }

        result = {min, max, maxLen};
    } else {
        float min = 0, max = 0;
        size_t maxLen = 0;

        for (; b != e; ++b) {
            if (*b < min) min = *b;
            if (*b > max) max = *b;
            const auto len = strLength(*b);
            if (maxLen < len) maxLen = len;
        }

        result.maxLen = maxLen;

        result.min.x = 0;
        result.max.x = std::distance(b, e);
        result.min.y = min;
        result.max.y = max;
    }
    return result;
}

template <class Iter>
auto plotPoints(Iter b, Iter e, std::vector<std::string>& graph,
                const MinMaxXY& o, const PlotDimension& d, char marker) {
    for (; b != e; b++) {
        const auto x = my::map<float>(b->x, o.min.x, o.max.x,
                                      o.maxLen + 6, d.width + o.maxLen - 1, true);
        const auto y = my::map<float>(b->y, o.min.y, o.max.y,
                                      d.height - 1, 0, true);
        graph.at(y).at(x) = marker;  // ●.
    }
}

template <class Iter>
auto plotVals(Iter b, Iter e, std::vector<std::string>& graph,
              const MinMaxXY& o, const PlotDimension& d, char marker) {
    for (size_t i = 0; b != e; b++, i++) {
        const auto x = my::map<float>(i, o.min.x, o.max.x,
                                      o.maxLen + 6, d.width + o.maxLen - 1, true);
        const auto y = my::map<float>(*b, o.min.y, o.max.y,
                                      d.height - 1, 0, true);
        graph.at(y).at(x) = marker;  // ●.
    }
}

};  // namespace detail

/**
 * @brief Prints plot into os stream
 * @note range min x and y has to be different from range max x and y
 *
 *      50.00  ┤   ╭──╮
 *      40.00  ┤  ╭╯  ╰╮
 *      30.00  ┤ ╭╯    ╰╮
 *      20.00  ┤╭╯      ╰╮
 *      10.00  ┼╯        ╰
 *
 * @tparam Iter iterator of PlotPoint or float underlying container
 * @param os stream to print to
 * @param b begin of container
 * @param e end of container
 * @param d dimensions by default .width = 50 .height = 20
 */
template <std::input_iterator Iter>
auto plot(std::ostream& os, Iter b, Iter e, const PlotDimension& d = {}, char marker = '*') {
    assert(d.width and d.height);
    // SET_UTF8_CONSOLE_CP();

    const auto o = detail::getMinMaxXY(b, e);
    const size_t curvy = static_cast<size_t>(Style::Curvy);

    assert(not my::same(o.min.x, o.max.x) and
           not my::same(o.min.y, o.max.y));

    // creating whiteboard
    std::vector<std::string> graph;
    graph.reserve(d.height);
    for (size_t i = 0; i < d.height; i++) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2)
           << std::right << std::setw(o.maxLen)
           << my::map<float>(i, 0, d.height - 1, o.max.y, o.min.y)
           << "  " << styles[curvy][7];

        graph.push_back(ss.str());
        graph[i].resize(d.width + o.maxLen, ' ');
    }

    // TODO use braille font to print graph
    // TODO figure out how to print braille font
    if constexpr (std::is_same_v<typename std::iterator_traits<Iter>::value_type, PlotPoint>)
        detail::plotPoints(b, e, graph, o, d, marker);
    else
        detail::plotVals(b, e, graph, o, d, marker);

    // printing whiteboard
    for (const auto& row : graph) os << row << '\n';

    // printing lower labels
    std::vector<std::string> xLabels;
    xLabels.reserve(d.width);

    size_t longestLabel = 0;
    for (size_t i = 0; i < d.width; i++) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(2)
           << my::map<float>(i, 0, d.width - 1, o.min.x, o.max.x);

        const auto label = ss.str();

        xLabels.push_back(label);

        if (longestLabel < label.size())
            longestLabel = label.size();
    }

    const auto yPad = std::string(o.maxLen + 3, ' ');

    // lines FIXME?
    os << yPad;
    for (size_t i = 0; i < xLabels.size(); i += longestLabel + 1) {
        os << my::styles[curvy][3];

        for (size_t j = 0; j < longestLabel; j++) {
            os << my::styles[curvy][0];
        }
    }

    // values
    my::printf(os, "\n{}", yPad);
    for (size_t i = 0; i < xLabels.size(); i += longestLabel + 1) {
        os << std::left << std::setw(longestLabel + 1) << xLabels[i];
    }

    my::printf("\n");
}

/**
 * @brief Plots data using two input iterators data, can contain Points or floats
 *
 * @tparam Iter iterator type
 * @param b begin of range
 * @param e end of range
 * @param d dimensions
 */
template <std::input_iterator Iter>
auto plot(Iter b, Iter e, const PlotDimension& d = {}, char marker = '*') {
    plot(std::cout, b, e, d, marker);
}

/**
 * @brief Plots data using generic container, can contain Points or floats
 *
 * @tparam Container any iterable type with begin() and end() functions provided
 * @param c container itself
 * @param d dimensions
 */
template <class Container>
auto plot(std::ostream& os, const Container& c, const PlotDimension& d = {},
          char marker = '*') {
    using std::begin;
    using std::end;
    plot(os, begin(c), end(c), d, marker);
}

/**
 * @brief Plots data using generic container, can contain Points or floats
 *
 * @tparam Container any iterable type with begin() and end() functions provided
 * @param c container itself
 * @param d dimensions
 */
template <class Container>
auto plot(const Container& c, const PlotDimension& d = {}, char marker = '*') {
    plot(std::cout, c.begin(), c.end(), d, marker);
}

/**
 * @brief Prints plot using generator function
 *
 * @tparam Generator Functor that returns float and takes float
 * @see RequireFloat<std::invoke_result_t<Generator, float>>
 *
 * @param os stream where to print
 * @param f generator functor example: [](auto e){ return std::sin(e); }
 * @param range .min, .max, .step values
 * @param d dimensions, by default .width = 50 .height = 20
 */
template <class Generator>
auto plot(std::ostream& os,
          Generator f, const NumericRange& range, const PlotDimension& d = {},
          char marker = '*') {
    static_assert(std::floating_point<std::invoke_result_t<Generator, float>>);

    std::vector<PlotPoint> points;

    const size_t toReserve = std::abs(range.max - range.min) / range.step;
    points.reserve(toReserve);

    for (auto x = range.min; x <= range.max; x += range.step) {
        points.emplace_back(x, f(x));
    }

    plot(os, points.begin(), points.end(), d, marker);
}

/**
 * @brief Prints plot using generator function
 *
 * @tparam Generator Functor that returns float and takes float
 * @see RequireFloat<std::invoke_result_t<Generator, float>>
 *
 * @param f generator functor example: [](auto e){ return std::sin(e); }
 * @param range .min, .max, .step values
 * @param d dimensions, by default .width = 50 .height = 20
 */
template <class Generator>
auto plot(Generator f, const NumericRange& range, const PlotDimension& d = {},
          char marker = '*') {
    static_assert(std::floating_point<std::invoke_result_t<Generator, float>>);
    plot(std::cout, f, range, d, marker);
}

}  // namespace fmt

}  // namespace my