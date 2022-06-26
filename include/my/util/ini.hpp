#include <bitset>
#include <cassert>
#include <map>
#include <my/format/format.hpp>
#include <my/format/println.hpp>
#include <my/util/functional.hpp>
#include <my/util/num_parser.hpp>
#include <ranges>
#include <sstream>
#include <variant>

namespace my {

class ini {
   public:
    using bool_t = bool;
    using float_t = double;
    using int_t = int64_t;
    using string_t = std::string;

    struct null_t {
        template <class Ch, class Tr>
        friend auto& operator<<(std::basic_ostream<Ch, Tr>& os,
                                const null_t& obj) {
            os << "null";
            return os;
        }
    };

    using key_t = string_t;
    using value_t = std::variant<null_t, bool_t, float_t, int_t, string_t>;
    using container_t = std::map<key_t, value_t>;

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
        auto value_visitor = my::overload(
            [&os](bool_t val) { os << (val ? "true" : "false"); },
            [&os](null_t val) { os << "null"; },
            [&os](int_t val) { os << val; },
            [&os](float_t val) { os << val; },
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

    // TODO test the shit of me

    /**
     * @brief Explicit call to parsing function
     *
     * @param is reference to stream from which to parse data
     */
    void read(std::istream& is) {
        enum {
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
        } state = begin;

        static auto isSectionOpen = [](char_t ch) -> bool { return ch == '['; };
        static auto isSectionClose = [](char_t ch) -> bool { return ch == ']'; };
        static auto isQuote = [](char_t ch) -> bool { return ch == '"'; };
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
        static auto isExponent = [](char_t ch) -> bool {
            return ch == 'E' or ch == 'e';
        };
        static auto isDot = [](char_t ch) -> bool { return ch == '.'; };
        static auto isMinus = [](char_t ch) -> bool { return ch == '-'; };
        static auto isPlus = [](char_t ch) -> bool { return ch == '+'; };
        static auto isUnderscore = [](char_t ch) -> bool { return ch == '_'; };
        static auto isAlpha = [](char_t ch) -> bool {
            return (ch >= 'a' and ch <= 'z') or  // lower
                   (ch >= 'A' and ch <= 'Z');    // upper
        };
        static auto isEndline = [](char_t ch) -> bool { return ch == '\n'; };
        static auto isBackslash = [](char_t ch) -> bool { return ch == '\\'; };
        static auto isComment = [](char_t ch) -> bool { return ch == ';' or
                                                               ch == '#'; };
        static auto isSpace = [](char_t ch) -> bool { return ch == ' ' or
                                                             ch == '\t'; };
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
        static auto isNull = [](const auto& value) -> bool {
            return value == "null";
        };

        struct {
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

        static auto regularTokenEndHandle =
            [&state, &current](char_t ch, auto finalize) -> bool {
            static auto finalizeImpl = [&finalize, &current]() {
                finalize();
                current.resetKeyValue();
            };

            if (isSpace(ch)) {
                finalizeImpl();
                state = consume_trailing_space;
                return true;
            }

            if (isComment(ch)) {
                finalizeImpl();
                state = comment;
                return true;
            }

            if (isEndline(ch)) {
                finalizeImpl();
                state = maybe_empty_line;
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

            switch (state) {
                case begin: {
                    if (isEndline(ch) or isSpace(ch)) {
                        break;
                    }

                    if (isComment(ch)) {
                        state = comment;
                        break;
                    }

                    if (isSectionOpen(ch)) {
                        state = section;
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
                        state = consume_trailing_space;
                        break;
                    }

                    if (not(isAlpha(ch) or isDigit(ch))) {
                        throw std::runtime_error(my::format(
                            "Section name must contain only alpha numeric chars:{}:{}",
                            current.line, current.pos));
                    }

                    current.section.push_back(ch);
                    break;
                }

                case maybe_empty_line: {
                    if (isEndline(ch) or isSpace(ch)) break;
                    if (isSectionOpen(ch)) {
                        current.resetSection();
                        state = section;
                        break;
                    }
                    if (isComment(ch)) {
                        state = comment;
                        break;
                    }
                    current.key.push_back(ch);
                    state = key;
                    break;
                }

                case consume_trailing_space: {
                    if (isSpace(ch)) break;
                    if (isComment(ch)) {
                        state = comment;
                        break;
                    }
                    if (isEndline(ch)) {
                        state = maybe_empty_line;
                        break;
                    }
                    throw std::runtime_error(
                        my::format("Only trailing spaces, comment or newline is required after value:{}:{}",
                                   current.line, current.pos));
                }

                case key: {
                    if (isComment(ch)) {
                        throw std::runtime_error(
                            my::format("Comments is prohibited inside of key declaration:{}:{}",
                                       current.line, current.pos));
                    }

                    if (isSpace(ch)) {
                        if (current.key.empty()) {
                            throw std::runtime_error(
                                my::format("Key must not be empty:{}:{}",
                                           current.line, current.pos));
                        }
                        state = key_value_delim;
                        break;
                    }

                    if (isEquals(ch)) {
                        if (current.key.empty()) {
                            throw std::runtime_error(
                                my::format("Key must not be empty:{}:{}",
                                           current.line, current.pos));
                        }
                        state = value_begin;
                        break;
                    }

                    if (not(isAlpha(ch) or isDigit(ch) or isUnderscore(ch))) {
                        throw std::runtime_error(
                            my::format("Key must contain only alpha numeric chars:{}:{}",
                                       current.line, current.pos));
                    }

                    current.key.push_back(ch);
                    break;
                }

                case key_value_delim: {
                    if (isSpace(ch)) break;

                    if (isEquals(ch)) {
                        state = value_begin;
                        break;
                    }

                    throw std::runtime_error(
                        my::format("Key must not contain spaces:{}:{}",
                                   current.line, current.pos));
                }

                case value_begin: {
                    if (isSpace(ch)) break;

                    if (regularTokenEndHandle(ch, [&current, this]() {
                            _sections[current.section][current.key] = null_t{};
                        })) break;

                    if (isQuote(ch)) {
                        state = string;
                        break;
                    }

                    if (isHexStart(ch, is.peek())) {
                        is.get();
                        state = hex_integer;
                        break;
                    }

                    if (isBinStart(ch, is.peek())) {
                        is.get();
                        state = bin_integer;
                        break;
                    }

                    if (isOctStart(ch, is.peek())) {
                        is.get();
                        state = oct_integer;
                        break;
                    }

                    if (isDigit(ch) or isMinus(ch) or isPlus(ch)) {
                        current.value.push_back(ch);
                        state = integer;
                        break;
                    }

                    if (isDot(ch)) {
                        current.value.push_back(ch);
                        state = floating;
                        break;
                    }

                    if (isTrueStart(ch) or isFalseStart(ch)) {
                        current.value.push_back(ch);
                        state = boolean;
                        break;
                    }

                    if (isNullStart(ch)) {
                        current.value.push_back(ch);
                        state = null_value;
                        break;
                    }

                    throw std::runtime_error(my::format(
                        "Value must be either quoted string, number, boolean, or null (empty line):{}:{}",
                        current.line, current.pos));
                }

                case comment: {
                    if (isEndline(ch)) {
                        state = maybe_empty_line;
                    }
                    break;
                }

                case string: {
                    if (isQuote(ch)) {
                        _sections[current.section][current.key] = current.value;
                        current.resetKeyValue();
                        state = consume_trailing_space;
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
                    if (regularTokenEndHandle(ch, [&current, this]() {
                            std::istringstream ss(current.value);
                            float_t result;
                            ss >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid floating point value:{}:{}",
                                    current.value, current.line, current.pos));
                            }
                            _sections[current.section][current.key] = result;
                        })) break;

                    if (not(isDigit(ch) or isExponent(ch))) {
                        throw std::runtime_error(my::format(
                            "Floating point number must only contain digits [0 - 9] or dot '.':{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case integer: {
                    if (regularTokenEndHandle(ch, [&current, this]() {
                            std::istringstream ss(current.value);
                            int_t result;
                            ss >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid integral value:{}:{}",
                                    current.value, current.line, current.pos));
                            }
                            _sections[current.section][current.key] = result;
                        })) break;

                    if (isDot(ch)) {
                        current.value.push_back(ch);
                        state = floating;
                        break;
                    }

                    if (not isDigit(ch)) {
                        throw std::runtime_error(my::format(
                            "Integer must only contain digits in range [0 - 9]:{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case bin_integer: {
                    if (regularTokenEndHandle(ch, [&current, this]() {
                            std::bitset<32> tmp(current.value);
                            _sections[current.section][current.key] = tmp.to_ulong();
                        })) break;

                    if (not isBinDigit(ch)) {
                        throw std::runtime_error(my::format(
                            "Binary integer must only contain 0 and 1 digits:{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case oct_integer: {
                    if (regularTokenEndHandle(ch, [&current, this]() {
                            std::istringstream ss(current.value);
                            int_t result;
                            ss >> std::oct >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid octal value:{}:{}",
                                    current.value, current.line, current.pos));
                            }
                            _sections[current.section][current.key] = result;
                        })) break;

                    if (not isOctDigit(ch)) {
                        throw std::runtime_error(my::format(
                            "Octal integer must only contain digits in range [0 - 7]:{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case hex_integer: {
                    if (regularTokenEndHandle(ch, [&current, this]() {
                            std::istringstream ss(current.value);
                            int_t result;
                            ss >> std::hex >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid hexadecimal value:{}:{}",
                                    current.value, current.line, current.pos));
                            }
                            _sections[current.section][current.key] = result;
                        })) break;

                    if (not isHexDigit(ch)) {
                        throw std::runtime_error(my::format(
                            "Hexadecimal integer must only contain digits in range [0 - 9] and chars in range [A - F]:{}:{}",
                            current.line, current.pos));
                    }

                    current.value.push_back(ch);
                    break;
                }

                case boolean: {
                    if (regularTokenEndHandle(ch, [&current, this]() {
                            std::istringstream ss(current.value);
                            bool_t result;
                            ss >> std::boolalpha >> result;
                            if (not ss) {
                                throw std::runtime_error(my::format(
                                    "Value \"{}\" is invalid boolean value:{}:{}",
                                    current.value, current.line, current.pos));
                            }
                            _sections[current.section][current.key] = result;
                        })) break;

                    current.value.push_back(ch);
                    break;
                }

                case null_value: {
                    if (regularTokenEndHandle(ch, [&current, this]() {
                            if (not isNull(current.value)) {
                                throw std::runtime_error(
                                    my::format(
                                        "Value \"{}\" is invalid null value:{}:{}",
                                        current.value, current.line, current.pos));
                            }

                            _sections[current.section][current.key] = null_t{};
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
    std::map<key_t, container_t> _sections;
};

inline namespace literals {

inline namespace ini_literals {

ini operator"" _ini(const char* data, size_t) {
    ini result(data);
    return result;
}

}  // namespace ini_literals

}  // namespace literals

}  // namespace  my