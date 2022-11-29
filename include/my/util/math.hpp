#pragma once

#include <my/util/concepts.hpp>  // my::arithmetic
//
#include <cassert>   // assert
#include <chrono>    // std::chrono::high_resolution_clock
#include <cmath>     // all
#include <concepts>  // std::floating_point, std::integral
#include <cstring>   // std::memcpy
#include <random>    // std::default_random_engine

#define FP std::floating_point

namespace my {

using f32_t = float;
using f64_t = double;
using f128_t = long double;

inline namespace math_defines {

template <FP T>
constexpr T E_V = 2.718'281'828'459'045'235'40;
template <FP T>
constexpr T LOG2E_V = 1.442'695'040'888'963'407'40;
template <FP T>
constexpr T LOG10E_V = 0.434'294'481'903'251'827'65;
template <FP T>
constexpr T LN2_V = 0.693'147'180'559'945'309'42;
template <FP T>
constexpr T LN10_V = 2.302'585'092'994'045'684'02;
template <FP T>
constexpr T E_GAMMA_V = 0.577'215'664'901'532'860'60;

template <FP T>
constexpr T TAU_V = 6.283'185'307'179'586'231'99;
template <FP T>
constexpr T PHI_V = 1.618'033'988'749'894'848'20;
template <FP T>
constexpr T QUARTER_PI_V = 0.785'398'163'397'448'309'62;
template <FP T>
constexpr T HALF_PI_V = 1.570'796'326'794'896'619'23;
template <FP T>
constexpr T PI_V = 3.141'592'653'589'793'238'46;
template <FP T>
constexpr T TWO_PI_V = 6.283'185'307'179'586'231'99;
template <FP T>
constexpr T INV_PI_V = 0.318'309'886'183'790'671'54;
template <FP T>
constexpr T INV_SQRT_PI_V = 1.128'379'167'095'512'573'90;
template <FP T>
constexpr T INV_TWO_PI_V = 0.636'619'772'367'581'343'08;

template <FP T>
constexpr T SQRT2_V = 1.414'213'562'373'095'048'80;
template <FP T>
constexpr T SQRT3_V = 1.732'050'807'568'877'193'17;
template <FP T>
constexpr T INV_SQRT2_V = 0.707'106'781'186'547'524'40;
template <FP T>
constexpr T INV_SQRT3_V = 0.577'350'269'189'625'842'08;

constexpr double E = 2.718'281'828'459'045'235'40;
constexpr double LOG2E = 1.442'695'040'888'963'407'40;
constexpr double LOG10E = 0.434'294'481'903'251'827'65;
constexpr double LN2 = 0.693'147'180'559'945'309'42;
constexpr double LN10 = 2.302'585'092'994'045'684'02;
constexpr double E_GAMMA = 0.577'215'664'901'532'860'60;

constexpr double TAU = 6.283'185'307'179'586'231'99;
constexpr double PHI = 1.618'033'988'749'894'848'20;
constexpr double QUARTER_PI = 0.785'398'163'397'448'309'62;
constexpr double HALF_PI = 1.570'796'326'794'896'619'23;
constexpr double PI = 3.141'592'653'589'793'238'46;
constexpr double TWO_PI = 6.283'185'307'179'586'231'99;
constexpr double INV_PI = 0.318'309'886'183'790'671'54;
constexpr double INV_SQRT_PI = 1.128'379'167'095'512'573'90;
constexpr double INV_TWO_PI = 0.636'619'772'367'581'343'08;

constexpr double SQRT2 = 1.414'213'562'373'095'048'80;
constexpr double SQRT3 = 1.732'050'807'568'877'193'17;
constexpr double INV_SQRT2 = 0.707'106'781'186'547'524'40;
constexpr double INV_SQRT3 = 0.577'350'269'189'625'842'08;

}  // namespace math_defines

// -----------------------// Scalar functions //----------------------- //

/**
 * @brief Converts a quantity specified in radians into degrees.
 *
 * @tparam T any floating point type
 * @param radians The quantity, in radians, to be converted to degrees.
 * @return 180 * radians / PI.
 */
template <FP T>
constexpr auto degrees(T radians) noexcept -> T {
    return 180.0 * radians / PI_V<T>;
}

/**
 * @brief Converts a quantity specified in radians into degrees.
 *
 * @tparam T any floating point type
 * @param degrees The quantity, in degrees, to be converted to radians.
 * @return PI * degrees / 180
 */
template <FP T>
constexpr auto radians(T degrees) noexcept -> T {
    return PI_V<T> * degrees / 180.0;
}

namespace math_literals {

constexpr f128_t operator"" _to_deg(f128_t rad) {
    return degrees<f128_t>(rad);
}

constexpr f128_t operator"" _to_rad(f128_t deg) {
    return radians<f128_t>(deg);
}

}  // namespace math_literals

namespace detail {

template <FP T>
struct PolarToCartesianResult {
    T x, y;
};

template <FP T>
struct CartesianToPolarResult {
    T radius, angle;
};

};  // namespace detail

/**
 * @brief Converts radius and angle into x and y in
 * the cartesian coordinate system
 *
 * @tparam T numeric type
 * @param radius polar radius
 * @param angle polar angle
 * @return PolarToCartesianResult<T> structure with .x and .y
 */
template <FP T, FP U>
constexpr auto polarToCartesian(T radius, U angle) noexcept
    -> detail::PolarToCartesianResult<std::common_type_t<T, U>> {
    return {.x = radius * std::cos(angle), .y = radius * std::sin(angle)};
}

/**
 * @brief  Converts x and y into radius and angle in
 * the polar coordinate system
 *
 * @tparam T numeric type
 * @param x cartesian x coordinate
 * @param y cartesian y coordinate
 * @return CartesianToPolarResult<T> structure with
 * .radius and .angle members
 */
template <FP T, FP U>
constexpr auto cartesianToPolar(T x, U y) noexcept
    -> detail::CartesianToPolarResult<std::common_type_t<T, U>> {
    return {.radius = std::hypot(x, y), .angle = std::atan(x / y)};
}

/**
 * @brief Clamps value in range (from, to)
 *
 * @tparam T any arithmetic type
 * @param n number to clamp
 * @param from lower bound
 * @param to higher bound
 * @return clamped value
 */
template <FP T, FP U, FP V>
constexpr auto clamp(T n, U from, V to) noexcept
    -> std::common_type_t<T, U, V> {
    return n > from ? from
           : n < to ? to
                    : n;
}

/**
 * @brief Re-maps a number from one range to another.
 * @note P5.js map
 *
 * @tparam T any arithmetic type
 * @param n the incoming value to be converted
 * @param start1 lower bound of the value's current range
 * @param stop1 upper bound of the value's current range
 * @param start2 lower bound of the value's target range
 * @param stop2 upper bound of the value's target range
 * @param withinBounds constrain the value to the newly mapped range
 * @return Remapped number
 */
template <FP T, FP U, FP V, FP W, FP X>
constexpr auto
map(T n, U start1, V stop1, W start2, X stop2,
    bool withinBounds = false) noexcept
    -> std::common_type_t<T, U, V, W, X> {
    const auto newval = (n - start1) /
                            (stop1 - start1) *
                            (stop2 - start2) +
                        start2;

    return not withinBounds ? newval
           : start2 < stop2 ? clamp(newval, start2, stop2)
                            : clamp(newval, stop2, start2);
}

/**
 * @brief Linearly interpolate between two values
 *
 * @tparam T any arithmetic type
 * @param x The start of the range in which to interpolate.
 * @param y The end of the range in which to interpolate.
 * @param t The value to use to interpolate between x and y.
 * @return Interpolated value
 */
template <FP T, FP U, FP V>
constexpr auto lerp(T x, U y, V t) noexcept
    -> std::common_type_t<T, U, V> {
    if ((x <= 0 and y >= 0) or (x >= 0 and y <= 0)) {
        return x * (1 - t) + y * t;
    }
    if (t == 1) return y;

    const std::common_type_t<T, U, V> res = x + t * (y - x);
    return (t > 1) == (y > x) ? (y < res ? res : y)
                              : (y > res ? res : y);
}

/**
 * @brief Perform Hermite interpolation between two values
 * @note glsl smoothstep
 *
 * @tparam T any arithmetic type
 * @param edge0 The value of the lower edge of the Hermite function.
 * @param edge1 The value of the upper edge of the Hermite function.
 * @param x The source value for interpolation.
 * @return Interpolated value
 */
template <FP T, FP U, FP V>
constexpr auto smoothstep(T edge0, U edge1, V x) noexcept
    -> std::common_type_t<T, U, V> {
    constexpr std::common_type_t<T, U, V> t =
        clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

/**
 * @brief Generates a step function by comparing x to edge. (aka saturation)
 * @note glsl step
 *
 * @tparam T any arithmetic type
 * @param edge The location of the edge of the step function.
 * @param x Specified value.
 * @return 0.0 if x < edge, and 1.0 otherwise.
 */
template <FP T, FP U>
constexpr auto step(T edge, U x) noexcept -> std::common_type_t<T, U> {
    return x < edge ? 0 : 1;
}

/**
 * @brief Clamps the specified value within the range of 0.0 to 1.0
 * @note glsl saturate
 *
 * @tparam T any arithmetic type
 * @param edge The location of the edge of the step function.
 * @param x Specified value.
 * @return clamped value
 */
template <FP T>
constexpr auto saturate(T x) noexcept -> T {
    return (x < 0)   ? 0
           : (x > 1) ? 1
                     : x;
}

/**
 * @brief Computes inverse sqrt using safe version of quakes III
 * Q_rsqrt function
 *
 * @param x number to compute inverse square root from
 * @return inv sqrt value
 */
template <FP T>
constexpr auto qrsqrt(T x) noexcept -> T {
    // (enable only on IEEE 754)
    static_assert(std::numeric_limits<T>::is_iec559);

    // long double is not yet supported
    static_assert(std::same_as<T, float> or
                  std::same_as<T, double>);

    if constexpr (std::same_as<T, float>) {
        float y = std::bit_cast<float>(
            0x5F3759DF - (std::bit_cast<uint32_t>(x) >> 1));

        return y * (1.5f - (x * 0.5f * y * y));
    }

    else /* std::same_as<T, double> */ {
        double y = std::bit_cast<double>(
            0x5fe6eb50c7aa19f9 - (std::bit_cast<uint64_t>(x) >> 1));
        //  0x5FE6EB50C7B537A9
        //  0x5FE6EC85E7DE30DA
        return y * (1.5 - (x * 0.5 * y * y));
    }
}

/**
 * @brief Computes inverse sqrt using std library,
 * effectively performs 1 / std::sqrt(x)
 *
 * @param x number to compute inverse square root from
 * @return inv sqrt value
 */
template <FP T>
constexpr auto rsqrt(T x) noexcept -> T { return 1.0 / std::sqrt(x); }

/**
 * @brief Generates pseudorandom number inclusively between low and high.
 * Uses high precision system time as seed and uniform distribution
 *
 * @tparam T target numeric type
 *
 * @return Pseudorandom number on closed range [low, high].
 */
template <my::arithmetic T = float, my::arithmetic U = T>
auto random(T low = 0, U high = 1) -> std::common_type_t<T, U> {
    // avoiding boilerplate)
    using clock = std::chrono::high_resolution_clock;
    using common_t = std::common_type_t<T, U>;

    // seeding from long time
    static uint_fast32_t seed = clock::now().time_since_epoch().count();
    // seeding generator
    static std::default_random_engine eng(seed);

    // determine what number type was passed
    // and using appropriate distribution
    if constexpr (std::is_integral_v<common_t>) {
        std::uniform_int_distribution<common_t> distribution(low, high);
        return distribution(eng);
    } else {
        std::uniform_real_distribution<common_t> distribution(low, high);
        return distribution(eng);
    }
}

/**
 * @brief Calculates factorial of integral number
 *
 * @tparam T target integral type
 * @param n number to take factorial from
 * @return Factorial of number n
 */
template <std::integral T>
constexpr auto fact(T n) noexcept -> T {
    if (!n) return T{1};

    auto result = n;
    while (--n) result *= n;

    return result;
}

namespace detail {

template <FP T>
struct _FractResult {
    T integral;
    T fractional;

    // this result can be casted down to fractional part
    // but we cannot ignore result of modf
    operator T() const { return fractional; }
};

}  // namespace detail

/**
 * @brief Computes fractional and integral part of the number
 * @note Result of this function has cast operator to fractional part
 *
 * @tparam T any floating point number type
 * @param n number to take fractional part from
 * @return The fractional part of x. This is calculated as x - floor(x).
 */
template <FP T>
constexpr auto fract(T n) noexcept -> detail::_FractResult<T> {
    T integral;
    T fractional = std::modf(n, &integral);
    return {.integral = integral,
            .fractional = fractional};
}

/**
 * @brief Computes a modulo b
 * @see glsl mod
 *
 * @tparam T any floating point number type
 * @param a first number
 * @param b second number
 * @return The value of a modulo b. This is computed as a - b * floor(a / b).
 */
template <FP T, FP U>
constexpr auto mod(T a, U b) noexcept -> std::common_type_t<T, U> {
    return a - b * std::floor(a / b);
}

/**
 * @brief Performs near same check of floating point number
 * @note http://realtimecollisiondetection.net/blog/?p=89
 *
 * @tparam T any floating point number type
 * @param a first number
 * @param b second number
 * @param epsilon difference to check for
 * std::numeric_limits<T>::epsilon by default
 * @return true if difference between numbers less than epsilon
 */
template <FP T>
constexpr auto same(T a, T b,
                    T epsilon = std::numeric_limits<T>::epsilon()) -> bool {
    constexpr T zero = 0.0;
    constexpr T one = 1.0;
    // assume small positive epsilon
    assert(epsilon >= zero and epsilon <= one);

    const T d = std::abs(a - b);
    const T maxAB = std::max(std::abs(a), std::abs(b));

    // if the multiply won't underflow then use a multiply
    if (maxAB >= one) {
        return d <= (epsilon * maxAB);
    }
    // multiply could underflow so use a divide if nonzero denominator
    if (maxAB > zero) {
        // correctly returns false on divide overflow
        // (inf <= epsilon is false), since overflow means the
        // relative difference is large and they are therefore not close
        return d / maxAB <= epsilon;
    }
    // both a and b are zero
    return true;
}

/**
 * @brief Performs near same check of floating point number
 * @note http://realtimecollisiondetection.net/blog/?p=89
 *
 * @tparam T any floating point number type
 * @param a first number
 * @param b second number
 * @param absTol absolute tolerance
 * @param relTol relative tolerance
 * @return true if difference between numbers less than epsilon
 */
template <FP T>
constexpr auto same(T a, T b, T absTol, T relTol) -> bool {
    /**
     * from http://realtimecollisiondetection.net/blog/?p=89
     * ...The absolute tolerance test fails when a and b become large,
     * and the relative tolerance test fails when they become small.
     * It is therefore desired to combine these two tests
     * together in a single test...
     */
    constexpr T d = std::abs(a - b);
    constexpr T maxAB = std::max(std::abs(a), std::abs(b));
    return d <= std::max(absTol, relTol * maxAB);
}

/**
 * @brief Extracts the sign of the parameter
 * @note glsl sign
 *
 * @tparam T any signed number type
 * @param x the value from which to extract the sign.
 * @return -1.0 if x is less than 0.0,
 *          0.0 if x is equal to 0.0,
 *         +1.0 if x is greater than 0.0.
 */
template <my::arithmetic T>
constexpr auto sign(T x) noexcept -> int32_t {
    if (x < 0) return -1;
    if (x > 0) return +1;
    return 0;
}

/**
 * @brief Returns sine random of seed number, can't be used as generator
 *
 * @tparam T any floating point number type
 * @param n seeder
 * @return random value computed by formula fract(sin(n) * 43758.5453123)
 */
template <FP T>
constexpr auto sinrand(T n) noexcept -> T {
    return fract<T>(std::sin(n) * 43758.5453123);
}

/**
 * @brief Returns one dimensional noise value
 *
 * @tparam T any floating point number type
 * @param p seeder
 * @return noised value
 */
template <FP T>
constexpr auto noise(T p) noexcept -> T {
    const T fl = std::floor(p);
    const T fc = p - fl;  // my::fract(p)
    return lerp<T>(sinrand<T>(fl), sinrand<T>(fl + 1.0), fc);
}

/**
 * @brief Returns jittered value of n
 *
 * @tparam T any floating point number type
 * @param n number to jit
 * @param j jitter force
 * @return jittered value
 */
template <FP T>
constexpr auto jitter(T n, T j) noexcept -> T {
    return n + j * my::random<T>();
}

/**
 * @brief Sine cardinal function
 * @see https://mathworld.wolfram.com/SincFunction.html
 *
 * @tparam T any floating point number type
 * @param x value in which compute sinc
 * @param k amount of 'bounces'
 * @return a phase shifted sin(x) value
 */
template <FP T>
constexpr auto sinc(T x, T k = 1.0) noexcept -> T {
    if (x == 0 or k == 0) return 1;
    const T a = k * PI_V<T> * x;
    return std::sin(a) / a;
}

/**
 * @brief Computes rectangular function
 *
 * @tparam T any floating point number type
 * @param x value to compute
 * @return rectangular function value at x
 */
template <FP T>
constexpr auto rect(T x) noexcept -> T {
    constexpr T half = 0.5;
    if (std::abs(x) < half) return 1.0;
    if (std::abs(x) > half) return 0.0;
    return half;
}

/**
 * @brief Returns absolute difference between two numbers
 *
 * @tparam T numeric type
 * @param lhs first number
 * @param rhs second number
 * @return positive absolute difference of two numbers
 */
template <my::arithmetic T>
constexpr auto diff(T lhs, T rhs) noexcept -> T {
    return lhs < rhs ? rhs - lhs : lhs - rhs;
}

/**
 * @brief Extended recursive Euclidean algorithm
 *
 * @param a first number to compute gcd from
 * @param b second number to compute gcd from
 * @param x Computed Bézout coefficient x
 * @param y Computed Bézout coefficient y
 * @return gcd of a and b
 */
constexpr int64_t xgcd(int64_t a, int64_t b,
                       int64_t &x, int64_t &y) {
    if (b == 0) {
        x = 1;
        y = 0;
        return a;
    }

    int64_t x1{}, y1{};
    const int64_t gcd = xgcd(b, a % b, x1, y1);  // recursive constexpr call
    x = y1;
    y = x1 - (a / b) * y1;
    return gcd;
}

/**
 * @brief Computes modular multiplicative inverse of two numbers
 *
 * @param a first number to compute
 * @param m second number to compute
 * @return int64_t modular multiplicative inverse
 */
constexpr int64_t modinv(int64_t a, int64_t m) {
    a %= m;
    for (int64_t x = 1; x < m; x++) {
        if ((a * x) % m == 1)
            return x;
    }
    return -1;
}

/**
 * @brief Computes binary n-th power of a in O(logN) complexity
 *
 * @param a base
 * @param n exponent
 * @return int64_t a to the power of n
 */
constexpr int64_t binpow(int64_t a, int64_t n) {
    int64_t res = 1;
    while (n) {
        if (n & 1)
            res *= a;
        a *= a;
        n >>= 1;
    }
    return res;
}

/**
 * @brief Computes binary n-th power of a modulo m in O(logN) complexity
 *
 * @param a base
 * @param n exponent
 * @param m modulo
 * @return int64_t a to the power of n modulo m
 */
constexpr int64_t binpow(int64_t a, int64_t n, int64_t m) {
    int64_t res = 1;
    while (n) {
        if (n & 1) {
            res *= a;
            res %= m;
        }
        a *= (a % m);
        a %= m;
        n >>= 1;
    }
    return res % m;
}

template <class PairT, class WidthT>
PairT twoDimensionalIndex(size_t index, WidthT width) {
    return {index % width, index / width};
}

template <class PairT, class WidthT, class CoordGetter>
size_t oneDimensionalIndex(PairT point, WidthT width,
                           CoordGetter first, CoordGetter second) {
    return first(point) + (second(point) * width);  // first / i / x, second / j / y
}

template <class PairT, class WidthT, class CoordGetter>
size_t oneDimensionalIndex(PairT point, WidthT width) {
    return point.x + (point.y * width);
}

template <my::arithmetic T>
constexpr auto isPowerOf2(T n) -> bool {
    if constexpr (std::is_integral_v<T>) {
        return not(n > 0 and n & (n - 1));
    } else {
        int32_t exponent;
        const T mantissa = std::frexp(n, &exponent);
        return mantissa == T{0.5};
    }
}

template <class F, FP T>
constexpr auto gradient(F &&f, T &&dx) noexcept {
    return [=]<std::convertible_to<T>... Xs>(Xs && ...xs) {
        std::array<T, sizeof...(xs)> res;
        const auto ddx = dx * 2;

        for (size_t i = 0; i < sizeof...(xs); ++i) {
            T params[]{std::forward<Xs>(xs)...};
            [&]<size_t... I>(std::index_sequence<I...>) {
                params[i] += dx;
                const auto f1 = f(params[I]...);
                params[i] -= ddx;
                const auto f2 = f(params[I]...);
                res[i] = (f1 - f2) / ddx;
            }
            (std::make_index_sequence<sizeof...(xs)>{});
        }

        return res;
    };
}

/**
 * @see https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
 *
 */
template <std::integral T>
constexpr void setBit(T &number, T bit) { number |= 1UL << bit; }

template <std::integral T>
constexpr void clearBit(T &number, T bit) { number &= ~(1UL << bit); }

template <std::integral T>
constexpr void toggleBit(T &number, T bit) { number ^= 1UL << bit; }

template <std::integral T>
constexpr bool checkBit(T number, T bit) { return (number >> bit) & 1U; }

}  // namespace my

#undef FP