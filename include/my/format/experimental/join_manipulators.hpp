#pragma once

#include <my/format/join.hpp>

namespace my::exp {

template <class P, class Ch, class Tr>
concept join_printer =
    requires(P printer, std::basic_ostream<Ch, Tr>& os, auto&& obj) {
    std::invoke(printer, os, std::forward<T>(obj));
};

template <class T, join_printer P>
concept join_manipulator = requires(T manip, P printer) {
    { std::invoke(manip, printer) } -> join_printer;
};

template <my::joinable T, join_printer Printer>
struct joiner {
   public:
    inline constexpr joiner(T&& val, Printer printer)
        : _val(std::forward<T>(val)), _printer(std::move(printer)) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os, const joiner& obj) {
        std::invoke(_printer, os, std::forward<T>(obj));
        return os;
    }

    template <join_manipulator Manip>
    friend auto operator|(joiner& join, Manip&& manip) {
        return std::invoke(std::forward<Manip>(manip), _printer);
    }

   private:
    T&& _val;
    Printer _printer;
};

template <my::joinable J, class Projection = std::identity>
struct join_base_printer {
};

template <my::iterable Range, class Projection = std::identity>
struct join_range_printer {
   public:
    using join_type = Range;

    template <class Ch, class Tr>
    auto operator()(std::basic_ostream<Ch, Tr>& os, Range&& obj) {
        using std::begin;
        using std::end;
        using std::prev;

        auto first = begin(obj);
        auto last = end(obj);

        os << open;

        if (first == last) {
            os << close;
            return os;
        }

        auto it = first;
        for (auto prev = prev(last); it != prev; ++it)
            os << join<Ch, Tr>(std::invoke(_proj, *it)) << delim;
        os << join<Ch, Tr>(std::invoke(_proj, *it));

        os << close;
        return os;
    }

   private:
    Projection _proj = {};

    const char* delim = my::delimiters<Range>::delim;
    const char* open = my::delimiters<Range>::open;
    const char* close = my::delimiters<Range>::close;
};

}  // namespace my::exp

#if 0

std::vector<std::stirng> vals{"string", "string", "string"};

println << join(vals) | eachQuoted;
println << join(vals) | eachProjected(&std::stirng::size);
println << join(vals) | quoted;
println << join(vals) | open("{") | delim("\n")  | close("}");

#endif