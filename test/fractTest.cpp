#include <my/format/format.hpp>
#include <my/util/math.hpp>

auto main() -> int {
    auto res = my::fract(10.134567);
    float fractional = my::fract(10.134567);

    my::printf("integral: {}\nfractional: {}\n\n", res.integral, res.fractional);
    my::printf("only fractional: {}\n", fractional);

    if (my::fract(10.134567) < 0.5) {
        my::printf("{}\n", "we where here");
    }

    //-------------do not write code after this line----------------
    system("pause > nul");
    return 0;
}