#pragma once
#ifndef MY_INI_HPP
#define MY_INI_HPP

#include <my/format/format.hpp>
//
#include <bitset>
#include <cassert>
#include <limits>
#include <map>
#include <ranges>
#include <sstream>
#include <variant>

namespace my {

namespace {

/**
 * @brief variant visitor overload
 *
 * @tparam Ts
 */
template <class... Ts>
struct overload : Ts... { using Ts::operator()...; };
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

/**
 * @brief Manipulator to print floats without zeroes at the end
 *
 * @tparam T
 */
template <std::floating_point T>
struct stripZeroes {
    constexpr explicit stripZeroes(T value)
        : _value(value) {
    }

    template <class Ch, class Tr>
    friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                            const stripZeroes& obj) {
        std::basic_stringstream<Ch, Tr> ss;

        ss << std::boolalpha
           << std::showpoint
           << std::setprecision(std::numeric_limits<T>::max_digits10)
           << obj._value;

        auto num = ss.str();
        num = num.substr(0, num.find_last_not_of('0') + 1);
        if (num.back() == '.') num.push_back('0');
        os << num;

        return os;
    }

   private:
    T _value;
};

}  // namespace

/**
 * @brief My .ini dialect file parser
 *
 * Data layout:
 *     All data layout starts from section declaration
 *       example:
 *         [Section]
 *
 *     You are allowed to use as many empty lines or spaces before and after
 *     section declaration as you want
 *
 *     Next major data layout is key-value pairs, it is written as follows
 *       example:
 *         key = *value*
 *
 *     The *value* can be either boolean, string, integer, floating point or null.
 *
 *     Comments are ';' or '#' symbol followed by any line of text and can appear
 *     anywhere else in the data layout except inside of section declaration or key-value pair
 *       example:
 *         ; comment here
 *         [Section] ; here
 *         ; comment here
 *         key = "value" ; here
 *         ; and here
 *
 *
 * Data types:
 *     null:
 *       Null values can be set in two ways, either by omitting value at all or
 *       explicitly writing "null" keyword
 *         example:
 *           [Nulls]
 *           noValueOmitted =
 *           noValue = null
 *
 *     boolean:
 *       Boolean value is represented either by "true" or "false" keywords
 *         example:
 *           [Booleans]
 *           bValue = true
 *           bValue2 = false

 *     integer:
 *       Integer is parsed with default c++ std::istream operator>> and can be either
 *       decimal, octal, binary or hexadecimal
 *       Also it is allowed to use prefix minus and plus
 *         example:
 *           [Integers]
 *           iDecimal = 42
 *           iHexadecimal = 0xDEADBEEF
 *           iOctal = 0o1471
 *           iBinary = 0b101010
 *           iNegative = -1022
 *           iPositive = +1011

 *     floating point:
 *       Floating point numbers as well as integers are parsed with default std::istream operator>>
 *       and can have plus minus sign, exponent and dot in the beginning and end of number
 *         example:
 *           [Floats]
 *           fFrontDot = .42
 *           fBackDot = 42.
 *           fExponent = 42e4
 *           fExponentWithDot = 4.2e4
 *           fLargeExponent = 4.2E2
 *           fNegative = -4.2
 *           fPositive = +4.2
 *
 *     string:
 *       String is represented as value inside of quotes (""), all chars inside of string read as-is,
 *       to preserve quote sign itself use \" construction.
 *       It behaves similarly to R() string literal except quote preservation
 *         example:
 *           [Strings]
 *           sValue = "The quick brown fox jumps over the \"lazy\" dog"
 *
 * Reading data from ini:
 *      All data inside of inner container is stored as std::variant so in order
 *      to retrieve data from it you need to use std::get function
 *      my::ini class declares usings for each type used.
 *        example:
 *          using my::literals;
 *          auto data = R"(
 *              [Floats]
 *              fFrontDot = .42
 *              fBackDot = 42.
 *              fExponent = 42e4
 *              fExponentWithDot = 4.2e4
 *              fLargeExponent = 4.2E2
 *              fNegative = -4.2
 *              fPositive = +4.2
 *          )"_ini;
 *          auto value = std::get<my::ini::float_t>(data["Floats"]["fNegative"]); // stores -4.2
 *
 * Parsing data:
 *      To parse data using ini class you have several possible possibilities.
 *      Using read() method with stream:
 *        my::ini usingReadMethod;
 *        std::ifstream in("ini-file.ini");
 *        usingReadMethod.read(in);
 *
 *      Using operator>>():
 *        my::ini usingOperator;
 *        in >> usingOperator;
 *
 *      Using read method with string:
 *        my::ini usingString;
 *        std::string str = "[Section]\nbValue = true";
 *        usingString.read(str);
 *
 *      Using constructor from stream:
 *        my::ini usingConstructor(in);
 *
 *      Using constructor from string:
 *        my::ini usingStrConstructor(in);
 *
 *  Serializing data:
 *      There is also several methods to serialize data back into string
 *
 *      Using write() method, it only accepts stream where to print:
 *        my::ini data;
 *        data.write(std::cout);
 *
 *      Using dump() method:
 *        my::ini data;
 *        std::string result = data.dump();
 *
 * Merge patch:
 *      Just like stl::map, my::ini also has method merge()
 *      The behaviour of this function is the following:
 *          If key was already in target ini file then it will be overriden
 *          with value of other ini, else it will be inserted
 *          If section wasn't in target file it will be copied completely
 *        example:
 *          auto first = R"(
 *              [Section1]
 *              value1 = true
 *
 *              [Section2]
 *              value1 = false
 *          )"_ini;
 *
 *          auto second = R"(
 *              [Section1]
 *              value1 = false
 *
 *              [Section3]
 *              value1 = null
 *          )"_ini;
 *
 *          first.merge(second);
 *
 *      After this operation contents of "first" is:
 *      [Section1]
 *      value1 = false
 *
 *      [Section2]
 *      value1 = false
 *
 *      [Section3]
 *      value1 = null
 *
 */
template <template <class, class> class Map = std::map>
class ini {
   public:
    using bool_t = bool;
    using float_t = double;
    using int_t = int64_t;
    using string_t = std::string;

    struct null_t {
        friend auto& operator<<(std::ostream& os, const null_t& obj) {
            os << "null";
            return os;
        }
    };

    using key_t = string_t;
    using value_t = std::variant<null_t, bool_t, float_t, int_t, string_t>;
    using container_t = Map<key_t, value_t>;

    using char_t = string_t::value_type;

   public:
    // constructors

    /**
     * @brief Construct empty ini object
     *
     */
    explicit ini() = default;

    /**
     * @brief Construct a new ini object with provided input stream to parse
     *
     * @param is input stream to parse
     */
    explicit ini(std::istream& is) { read(is); }

    /**
     * @brief Construct a new ini object with provided string data to parse
     *
     * @param data string to parse
     */
    explicit ini(const string_t& data) {
        std::stringstream ss(data);
        read(ss);
    }

    // convenience operators

    friend auto& operator<<(std::ostream& os, const ini& file) {
        file.write(os);
        return os;
    }

    friend auto& operator>>(std::istream& is, ini& file) {
        file.read(is);
        return is;
    }

    // access to particular section

    /**
     * @brief Access particular section
     * @note inserts empty section if there where no section with provided name
     *
     * @param section string key of section to search
     * @return container_t& key-value pairs of particular section
     */
    container_t& operator[](const key_t& section) {
        return _sections[section];
    }

    /**
     * @brief Access particular section
     * @note throws if there where no section with provided name
     *
     * @param section string key of section to search
     * @return const container_t& key-value pairs of particular section
     */
    const container_t& at(const key_t& section) const {
        return _sections.at(section);
    }

    /**
     * @brief Access particular section
     * @note throws if there where no section with provided name
     *
     * @param section string key of section to search
     * @return container_t& key-value pairs of particular section
     */
    container_t& at(const key_t& section) {
        return _sections.at(section);
    }

    /**
     * @brief Merge patch current file with other.
     * Each new value of section will be inserted, each different value will
     * be changed to new and each new section will be appended
     *
     * @param rhs other ini file to merge with
     * @return size_t amount of inserted and changed values
     */
    size_t merge(const ini& rhs) {
        size_t mutated = 0;

        for (auto&& section : rhs._sections) {
            auto it = _sections.find(section.first);
            if (it != _sections.end()) {
                for (auto&& keyValue : section.second) {
                    it->second.insert_or_assign(keyValue.first,
                                                keyValue.second);
                    ++mutated;
                }
                continue;
            }
            _sections[section.first] = section.second;
            mutated += section.second.size();
        }

        return mutated;
    }

    // access to internal buffer

    /**
     * @brief Access internal methods via pointer to std::map<key_t, container_t>
     */
    constexpr auto* operator->() { return &_sections; }
    /**
     * @brief Access internal methods via pointer to std::map<key_t, container_t>
     */
    constexpr const auto* operator->() const { return &_sections; }

    /**
     * @brief Access internal methods reference to std::map<key_t, container_t>
     */
    constexpr auto& data() { return _sections; }
    /**
     * @brief Access internal methods reference to std::map<key_t, container_t>
     */
    constexpr const auto& data() const { return _sections; }

    // serialize deserialize

    /**
     * @brief Explicit call to serialization function
     *
     * @param os reference to stream where to print data
     */
    void write(std::ostream& os) const {
        auto value_visitor = overload(
            [&os](bool_t val) { os << val; },
            [&os](null_t val) { os << val; },
            [&os](int_t val) { os << val; },
            [&os](float_t val) { os << stripZeroes(val); },
            [&os](const string_t& val) { os << std::quoted(val); });

        for (auto&& section : _sections) {
            my::printf(os, "[{}]\n", section.first);

            for (auto&& keyValue : section.second) {
                os << keyValue.first << " = ";
                std::visit(value_visitor, keyValue.second);

                os << '\n';
            }
            os << std::endl;
        }
    }

    /**
     * @brief Returns string representation of internal data
     *
     * @return std::string string representation of internal data
     */
    std::string dump() const {
        std::stringstream ss;
        write(ss);
        return ss.str();
    }

    // TODO test me

    /**
     * @brief Explicit call to parsing function
     *
     * @param is reference to stream from which to parse data
     */
    void read(std::istream& is) {
        static auto isLower = [](char_t ch) -> bool {
            return ch >= 'a' and ch <= 'z';
        };
        static auto isUpper = [](char_t ch) -> bool {
            return ch >= 'A' and ch <= 'Z';
        };
        static auto isAlpha = [](char_t ch) -> bool {
            return isLower(ch) or isUpper(ch);
        };
        static auto isBinDigit = [](char_t ch) -> bool {
            return ch == '1' or ch == '0';
        };
        static auto isOctDigit = [](char_t ch) -> bool {
            return ch >= '0' and ch <= '7';
        };
        static auto isDigit = [](char_t ch) -> bool {
            return ch >= '0' and ch <= '9';
        };
        static auto isHexDigit = [](char_t ch) -> bool {
            return isDigit(ch) or
                   (ch >= 'A' and ch <= 'F') or
                   (ch >= 'a' and ch <= 'f');
        };
        static auto isComment = [](char_t ch) -> bool { return ch == ';' or
                                                               ch == '#'; };
        static auto isSpace = [](char_t ch) -> bool { return ch == ' ' or
                                                             ch == '\t'; };
        static auto isExponent = [](char_t ch) -> bool {
            return ch == 'E' or ch == 'e';
        };

        static auto isSectionOpen = [](char_t ch) -> bool { return ch == '['; };
        static auto isSectionClose = [](char_t ch) -> bool { return ch == ']'; };
        static auto isQuote = [](char_t ch) -> bool { return ch == '"'; };
        static auto isDot = [](char_t ch) -> bool { return ch == '.'; };
        static auto isMinus = [](char_t ch) -> bool { return ch == '-'; };
        static auto isPlus = [](char_t ch) -> bool { return ch == '+'; };
        static auto isUnderscore = [](char_t ch) -> bool { return ch == '_'; };
        static auto isEndline = [](char_t ch) -> bool { return ch == '\n'; };
        static auto isBackslash = [](char_t ch) -> bool { return ch == '\\'; };
        static auto isEquals = [](char_t ch) -> bool { return ch == '='; };

        static auto isTrueStart = [](char_t ch) -> bool { return ch == 't'; };
        static auto isFalseStart = [](char_t ch) -> bool { return ch == 'f'; };
        static auto isNullStart = [](char_t ch) -> bool { return ch == 'n'; };

        static auto isHexStart = [](char_t ch, char_t next) -> bool {
            return ch == '0' and next == 'x';
        };
        static auto isBinStart = [](char_t ch, char_t next) -> bool {
            return ch == '0' and next == 'b';
        };
        static auto isOctStart = [](char_t ch, char_t next) -> bool {
            return ch == '0' and next == 'o';
        };

        enum State {
            maybe_empty_line,
            consume_trailing_space,
            begin,

            section,

            key,
            key_value_delim,
            comment,

            value_begin,
            string,
            floating,
            integer,
            bin_integer,
            oct_integer,
            hex_integer,
            boolean,
            null_value,
        };

        struct {
            State state = begin;
            string_t section = "";
            string_t key = "";
            string_t value = "";
            size_t line = 1;
            size_t pos = 1;

            void resetSection() {
                section.clear();
            }

            void resetKeyValue() {
                key.clear();
                value.clear();
            }

        } current;

        static auto regularTokenEndHandle = [](char_t ch, auto& current,
                                               auto finalize) -> bool {
            if (isSpace(ch)) {
                finalize(current);
                current.resetKeyValue();
                current.state = consume_trailing_space;
                return true;
            }

            if (isComment(ch)) {
                finalize(current);
                current.resetKeyValue();
                current.state = comment;
                return true;
            }

            if (isEndline(ch)) {
                finalize(current);
                current.resetKeyValue();
                current.state = maybe_empty_line;
                return true;
            }

            return false;
        };

        for (;;) {
            const auto ch = is.get();

            if (is.eof() or is.fail()) {
                break;
            }

            if (isEndline(ch)) {
                current.pos = 1;
                ++current.line;
            }
            ++current.pos;

            switch (current.state) {
                case begin: {
                    if (isEndline(ch) or isSpace(ch)) {
                        break;
                    }

                    if (isComment(ch)) {
                        current.state = comment;
                        break;
                    }

                    if (isSectionOpen(ch)) {
                        current.state = section;
                        break;
                    }

                    throw std::runtime_error(
                        my::format("File must start from section:{}:{}",
                                   current.line, current.pos));
                }

                case section: {
                    if (isSectionClose(ch)) {
                        if (current.section.empty()) {
                            throw std::runtime_error(my::format(
                                "Section name must not be empty:{}:{}",
                                current.line, current.pos));
                        }
                        current.state = consume_trailing_space;
                        break;
                    }

                    if (not(isAlpha(ch) or isDigit(ch))) {
                        throw std::runtime_error(my::format(
                            "Section name must contain only "
                            "alpha numeric chars:{}:{}",
                            current.line, current.pos));
                    }

                    current.section.push_back(ch);
                    break;
                }

                case maybe_empty_line: {
                    if (isEndline(ch) or isSpace(ch)) break;
                    if (isSectionOpen(ch)) {
                        current.resetSection();
                        current.state = section;
                        break;
                    }
                    if (isComment(ch)) {
                        current.state = comment;
                        break;
                    }
                    current.key.push_back(ch);
                    current.state = key;
                    break;
                }

                case consume_trailing_space: {
                    if (isSpace(ch)) break;
                    if (isComment(ch)) {
                        current.state = comment;
                        break;
                    }
                    if (isEndline(ch)) {
                        current.state = maybe_empty_line;
                        break;
                    }
                    throw std::runtime_error(
                        my::format("Only trailing spaces, comment or "
                                   "newline is required after value:{}:{}",
                                   current.line, current.pos));
                }

                case key: {
                    if (isComment(ch)) {
                        throw std::runtime_error(
                            my::format("Comments is prohibited inside "
                                       "of key declaration:{}:{}",
                                       current.line, current.pos));
                    }

                    if (isSpace(ch)) {
                        if (current.key.empty()) {
                            throw std::runtime_error(
                                my::format("Key must not be empty:{}:{}",
                                           current.line, current.pos));
                        }
                        current.state = key_value_delim;
                        break;
                    }

                    if (isEquals(ch)) {
                        if (current.key.empty()) {
                            throw std::runtime_error(
                                my::format("Key must not be empty:{}:{}",
                                           current.line, current.pos));
                        }
                        current.state = value_begin;
                        break;
                    }

                    if (not(isAlpha(ch) or isDigit(ch) or isUnderscore(ch))) {
                        throw std::runtime_error(
                            my::format("Key must contain only alpha "
                                       "numeric chars:{}:{}",
                                       current.line, current.pos));
                    }

                    current.key.push_back(ch);
                    break;
                }

                case key_value_delim: {
                    if (isSpace(ch)) break;

                    if (isEquals(ch)) {
                        current.state = value_begin;
                        break;
                    }

                    throw std::runtime_error(
                        my::format("Key must not contain spaces:{}:{}",
                                   current.line, current.pos));
                }

                case value_begin: {
                    if (isSpace(ch)) break;

                    if (regularTokenEndHandle(ch, current, [this](auto& curr) {
                            _sections[curr.section][curr.key] = null_t{};
                        })) break;

                    if (isQuote(ch)) {
                        current.state = string;
                        break;
                    }

                    if (isHexStart(ch, is.peek())) {
                        is.get();
                        current.state = hex_integer;
                        break;
                    }

                    if (isBinStart(ch, is.peek())) {
                        is.get();
                        current.state = bin_integer;
                        break;
                    }

                    if (isOctStart(ch, is.peek())) {
                        is.get();
                        current.state = oct_integer;
                        break;
                    }

                    if (isDigit(ch) or isMinus(ch) or isPlus(ch)) {
                        current.value.push_back(ch);
                        current.state = integer;
                        break;
                    }

                    if (isDot(ch)) {
                        current.value.push_back(ch);
                        current.state = floating;
                        break;
                    }

                    if (isTrueStart(ch) or isFalseStart(ch)) {
                        current.value.push_back(ch);
                        current.state = boolean;
                        break;
                    }

                    if (isNullStart(ch)) {
                        current.value.push_back(ch);
                        current.state = null_value;
                        break;
                    }

                    throw std::runtime_error(my::format(
                        "Value must be either quoted string, number, "
                        "boolean, or null (empty line):{}:{}",
                        current.line, current.pos));
                }

                case comment: {
                    if (isEndline(ch)) {
                        current.state = maybe_empty_line;
                    }
                    break;
                }

                case string: {
                    if (isQuote(ch)) {
                        _sections[current.section][current.key] = current.value;
                        current.resetKeyValue();
                        current.state = consume_trailing_space;
                        break;
                    }

                    if (isBackslash(ch) and isQuote(is.peek())) {
                        current.value.push_back(is.get());
                        break;
                    }

                    current.value.push_back(ch);
                    break;
                }

                case floating: {
                    if (regularTokenEndHandle(ch, current, [this](auto& curr) {
                            std::istringstream ss(curr.value);
                            float_t result;
                            ss >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid floating "
                                    "point value:{}:{}",
                                    curr.value, curr.line, curr.pos));
                            }
                            _sections[curr.section][curr.key] = result;
                        })) break;

                    if (not(isDigit(ch) or isExponent(ch) or
                            isPlus(ch) or isMinus(ch))) {
                        throw std::runtime_error(my::format(
                            "Invalid symbol \"{}\" in floating point "
                            "number:{}:{}",
                            ch, current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case integer: {
                    if (regularTokenEndHandle(ch, current, [this](auto& curr) {
                            std::istringstream ss(curr.value);
                            int_t result;
                            ss >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid integral "
                                    "value:{}:{}",
                                    curr.value, curr.line, curr.pos));
                            }
                            _sections[curr.section][curr.key] = result;
                        })) break;

                    if (isDot(ch)) {
                        current.value.push_back(ch);
                        current.state = floating;
                        break;
                    }

                    if (not isDigit(ch)) {
                        throw std::runtime_error(my::format(
                            "Integer must only contain digits "
                            "in range [0 - 9]:{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case bin_integer: {
                    if (regularTokenEndHandle(ch, current, [this](auto& curr) {
                            std::bitset<32> tmp(curr.value);
                            _sections[curr.section][curr.key] = tmp.to_ulong();
                        })) break;

                    if (not isBinDigit(ch)) {
                        throw std::runtime_error(my::format(
                            "Binary integer must only contain "
                            "0 and 1 digits:{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case oct_integer: {
                    if (regularTokenEndHandle(ch, current, [this](auto& curr) {
                            std::istringstream ss(curr.value);
                            int_t result;
                            ss >> std::oct >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid octal value:{}:{}",
                                    curr.value, curr.line, curr.pos));
                            }
                            _sections[curr.section][curr.key] = result;
                        })) break;

                    if (not isOctDigit(ch)) {
                        throw std::runtime_error(my::format(
                            "Octal integer must only contain "
                            "digits in range [0 - 7]:{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case hex_integer: {
                    if (regularTokenEndHandle(ch, current, [this](auto& curr) {
                            std::istringstream ss(curr.value);
                            int_t result;
                            ss >> std::hex >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid hexadecimal value:{}:{}",
                                    curr.value, curr.line, curr.pos));
                            }
                            _sections[curr.section][curr.key] = result;
                        })) break;

                    if (not isHexDigit(ch)) {
                        throw std::runtime_error(my::format(
                            "Hexadecimal integer must only contain digits "
                            "in range [0 - 9] and chars in range [A - F]:{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case boolean: {
                    if (regularTokenEndHandle(ch, current, [this](auto& curr) {
                            std::istringstream ss(curr.value);
                            bool_t result;
                            ss >> std::boolalpha >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid boolean value:{}:{}",
                                    curr.value, curr.line, curr.pos));
                            }
                            _sections[curr.section][curr.key] = result;
                        })) break;

                    current.value.push_back(ch);
                    break;
                }

                case null_value: {
                    if (regularTokenEndHandle(ch, current, [this](auto& curr) {
                            if (curr.value != "null") {
                                throw std::runtime_error(
                                    my::format(
                                        "Value \"{}\" is invalid null value:{}:{}",
                                        curr.value, curr.line, curr.pos));
                            }

                            _sections[curr.section][curr.key] = null_t{};
                        })) break;

                    current.value.push_back(ch);
                    break;
                }
            }
        }
    }

    /**
     * @brief Explicit call to parsing function
     *
     * @param data string to parse
     */
    void read(const std::string& data) {
        std::stringstream ss(data);
        read(ss);
    }

   private:
    Map<key_t, container_t> _sections;
};

inline namespace literals {

inline namespace ini_literals {

auto operator"" _ini(const char* data, size_t) {
    ini<std::map> result(data);
    return result;
}

}  // namespace ini_literals

}  // namespace literals

}  // namespace  my

#endif  // MY_INI_HPP