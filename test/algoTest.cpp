#include <my/format/println.hpp>
#include <my/util/algorithm.hpp>
#include <my/util/str_utils.hpp>

auto main() -> int {
    std::vector<int> a{1, 2, 3, 4, 5, 6, 7};
    std::vector<int> b{1, 2, 3, 4, 5, 6, 7};

    const auto aPlusBStrings =
        my::transform<std::vector<std::string>>(
            a, [](auto a, auto b) { return my::toString(a + b); },
            my::back_inserter{}, b);

    my::println << aPlusBStrings;

    //-------------do not write code after this line----------------
    system("pause > nul");
    return 0;
}