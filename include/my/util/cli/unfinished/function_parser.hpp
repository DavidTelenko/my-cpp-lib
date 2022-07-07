#include <my/format/format.hpp>
#include <unordered_map>

namespace my::cli {

template <std::floating_point T = double>
class FunctionParser {
   public:
    using string_t = std::string;
    using key_t = string_t;
    using float_t = T;

    using args_t = std::unordered_map<key_t, float_t>;
    using function_t = std::function<float_t(const args_t&)>;

    explicit FunctionParser() = default;

    friend auto& operator<<(std::ostream& os, const FunctionParser& parser) {
        parser.write(os);
        return os;
    }

    friend auto& operator>>(std::istream& is, FunctionParser& parser) {
        parser.read(is);
        return is;
    }

    auto read(std::istream& is) {
        enum State {};

        static const std::unordered_map<std::string, function_t> keywords {
        }
    }

    auto write(std::ostream& os) const {
    }

   private:
    std::unordered_map<key_t, function_t> functions_;
};

#if 0

{
    FunctionParser parser;
    std::string functions = R"(
      f(x) = x ** 2
      u(x) = exp(x) ** 2 * sin(x)
      l(x) = (x + 3) * 4 + 2 * exp(3)
      p(x, y) = pow(3, x) * y
    )";

    // del f

    std::istringstream is(functions);

    is >> parser;

    // map [name function] -> function

    // ! std::function != args...                   parsed function
    // ! void* / variant std::function...           parsed function
    // ! atd::any                                   parsed function

    // function <- arguments (string), { string expression + parsed function n... arguments // }

    auto res = parser["f"](10);
    res = parser["p"](10, 2);
}

#endif

}  // namespace my::cli