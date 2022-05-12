#include <my/format/format.hpp>
#include <my/format/table.hpp>
#include <my/util/functional.hpp>
#include <my/util/win_defines.hpp>
#include <ranges>
#include <sstream>

struct Person {
    uint16_t age;
    std::string name;
    std::string surname;
};

auto main() -> int {
    SET_UTF8_CONSOLE_CP();

    std::vector<Person> persons{
        Person(32, "John", "Week"),
        Person(23, "Anna", "Lene"),
        Person(53, "Corney", "Clarcson"),
        Person(12, "George", "Freeman"),
        Person(44, "Ken", "Thomson"),
        Person(42, "Lee", "James"),
        Person(36, "Railey", "Colonel"),
        Person(32, "James", "Jeen"),
        Person(21, "Oran", "Kaliss"),
        Person(43, "J", "P"),
    };

    std::ranges::sort(persons, my::compareProject(&Person::age)
                                   .thenProject(&Person::surname));

    my::table(persons,
              &Person::name,
              &Person::surname,
              &Person::age)
        .header("name", "surname", "age")
        .sameHeaderFooter()
        .print();

    //-------------do not write code after this line----------------
    system("pause > nul");
    return 0;
}