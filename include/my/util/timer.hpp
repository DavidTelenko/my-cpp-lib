#pragma once
#ifndef MY_TIMER_HPP
#define MY_TIMER_HPP

#include <my/format/format.hpp>
//
#include <cassert>
#include <chrono>

namespace my {

class Timer {
   public:
    Timer(std::ostream& os, std::string_view label = "unknown")
        : _os(os), _label(label) { start(); }

    Timer(std::string_view label = "unknown")
        : _os(std::cout), _label(label) { start(); }

    ~Timer() { stop(); }

    auto& setMessageFormat(std::string_view format) {
        _format = format;
        return *this;
    }

    void start() {
        using namespace std::chrono;
        _stopped = false;
        _lastTime = high_resolution_clock::now();
    }

    void stop() {
        using namespace std::chrono;

        if(_stopped) return;

        const auto diff = high_resolution_clock::now() - _lastTime;
        const auto milli = duration_cast<milliseconds>(diff).count();
        const auto nano = duration_cast<nanoseconds>(diff).count();

        struct ThousandsSep : std::numpunct<char> {
            char do_thousands_sep() const { return '\''; }
            std::string do_grouping() const { return "\3"; }
        };

        const auto prev =
            _os.imbue(std::locale(_os.getloc(), new ThousandsSep));
        my::printf(_os, _format.data(), _label, nano, milli);
        _os.imbue(prev);
        _stopped = true;
    }

   private:
    std::ostream& _os;
    std::string_view _label;
    std::string_view _format = "[{}] took: {} ns ({} ms)\n";
    std::chrono::_V2::system_clock::time_point _lastTime;
    bool _stopped = true;
};

}  // namespace my

#endif  // MY_TIMER_HPP