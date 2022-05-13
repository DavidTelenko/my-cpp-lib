#include <my/format/println.hpp>
#include <my/util/traits.hpp>

auto main() -> int {
    std::cout << my::printable<int>;

    //-------------do not write code after this line----------------
    system("pause > nul");
    return 0;
}