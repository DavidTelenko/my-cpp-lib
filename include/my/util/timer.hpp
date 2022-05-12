#pragma once
#ifndef MY_TIMER_HPP
#define MY_TIMER_HPP

#include <my/format/format.hpp>
//
#include <chrono>

namespace my {

class Timer {
    std::ostream& os;
    std::string_view label;
    std::string_view format = "[{}] took: {} ns ({} ms)\n";
    std::chrono::_V2::system_clock::time_point lastTime;

    auto start() {
        using namespace std::chrono;
        lastTime = high_resolution_clock::now();
    }

    auto stop() {
        using namespace std::chrono;
        const auto diff = high_resolution_clock::now() - lastTime;
        const auto milli = duration_cast<milliseconds>(diff).count();
        const auto nano = duration_cast<nanoseconds>(diff).count();

        struct ThousandsSep : std::numpunct<char> {
            char do_thousands_sep() const { return '\''; }
            std::string do_grouping() const { return "\3"; }
        };

        const auto prev = os.imbue(std::locale(os.getloc(), new ThousandsSep));
        my::printf(os, format.data(), label, nano, milli);
        os.imbue(prev);
    }

   public:
    Timer(std::ostream& os, std::string_view label = "unknown") : os(os), label(label) { start(); }
    Timer(std::string_view label = "unknown") : os(std::cout), label(label) { start(); }

    ~Timer() { stop(); }
};

}  // namespace my

#endif  // MY_TIMER_HPP