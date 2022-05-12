#pragma once
#ifndef MY_MATH_HPP
#define MY_MATH_HPP

// clib
#include <cassert>   // assert
#include <cmath>     // all inside
#include <concepts>  // std::floating_point std::integral
#include <cstring>   // std::memcpy
// cpplib
#include <chrono>  // for my::random
#include <random>  // for my::random
// mylib
#include <my/util/traits.hpp>  // my::arithmetic

namespace my {

using f32_t = float;
using f64_t = double;
using f128_t = long double;

inline namespace math_defines {

template <std::floating_point T>
inline constexpr T E_V = 2.718'281'828'459'045'235'40;
template <std::floating_point T>
inline constexpr T LOG2E_V = 1.442'695'040'888'963'407'40;
template <std::floating_point T>
inline constexpr T LOG10E_V = 0.434'294'481'903'251'827'65;
template <std::floating_point T>
inline constexpr T LN2_V = 0.693'147'180'559'945'309'42;
template <std::floating_point T>
inline constexpr T LN10_V = 2.302'585'092'994'045'684'02;
template <std::floating_point T>
inline constexpr T E_GAMMA_V = 0.577'215'664'901'532'860'60;

template <std::floating_point T>
inline constexpr T TAU_V = 6.283'185'307'179'586'231'99;
template <std::floating_point T>
inline constexpr T PHI_V = 1.618'033'988'749'894'848'20;
template <std::floating_point T>
inline constexpr T QUARTER_PI_V = 0.785'398'163'397'448'309'62;
template <std::floating_point T>
inline constexpr T HALF_PI_V = 1.570'796'326'794'896'619'23;
template <std::floating_point T>
inline constexpr T PI_V = 3.141'592'653'589'793'238'46;
template <std::floating_point T>
inline constexpr T TWO_PI_V = 6.283'185'307'179'586'231'99;
template <std::floating_point T>
inline constexpr T INV_PI_V = 0.318'309'886'183'790'671'54;
template <std::floating_point T>
inline constexpr T INV_SQRT_PI_V = 1.128'379'167'095'512'573'90;
template <std::floating_point T>
inline constexpr T INV_TWO_PI_V = 0.636'619'772'367'581'343'08;

template <std::floating_point T>
inline constexpr T SQRT2_V = 1.414'213'562'373'095'048'80;
template <std::floating_point T>
inline constexpr T SQRT3_V = 1.732'050'807'568'877'193'17;
template <std::floating_point T>
inline constexpr T INV_SQRT2_V = 0.707'106'781'186'547'524'40;
template <std::floating_point T>
inline constexpr T INV_SQRT3_V = 0.577'350'269'189'625'842'08;

inline constexpr double E = 2.718'281'828'459'045'235'40;
inline constexpr double LOG2E = 1.442'695'040'888'963'407'40;
inline constexpr double LOG10E = 0.434'294'481'903'251'827'65;
inline constexpr double LN2 = 0.693'147'180'559'945'309'42;
inline constexpr double LN10 = 2.302'585'092'994'045'684'02;
inline constexpr double E_GAMMA = 0.577'215'664'901'532'860'60;

inline constexpr double TAU = 6.283'185'307'179'586'231'99;
inline constexpr double PHI = 1.618'033'988'749'894'848'20;
inline constexpr double QUARTER_PI = 0.785'398'163'397'448'309'62;
inline constexpr double HALF_PI = 1.570'796'326'794'896'619'23;
inline constexpr double PI = 3.141'592'653'589'793'238'46;
inline constexpr double TWO_PI = 6.283'185'307'179'586'231'99;
inline constexpr double INV_PI = 0.318'309'886'183'790'671'54;
inline constexpr double INV_SQRT_PI = 1.128'379'167'095'512'573'90;
inline constexpr double INV_TWO_PI = 0.636'619'772'367'581'343'08;

inline constexpr double SQRT2 = 1.414'213'562'373'095'048'80;
inline constexpr double SQRT3 = 1.732'050'807'568'877'193'17;
inline constexpr double INV_SQRT2 = 0.707'106'781'186'547'524'40;
inline constexpr double INV_SQRT3 = 0.577'350'269'189'625'842'08;

}  // namespace math_defines

// -----------------------// Scalar functions //----------------------- //

/**
 * @brief Converts a quantity specified in radians into degrees.
 *
 * @tparam Number any arithmetic type
 * @param radians The quantity, in radians, to be converted to degrees.
 * @return 180 * radians / PI.
 */
template <std::floating_point Number = float>
constexpr Number degrees(Number radians) {
    return 180.0 * radians / PI_V<Number>;
}

/**
 * @brief Converts a quantity specified in radians into degrees.
 *
 * @tparam Number any arithmetic type
 * @param degrees The quantity, in degrees, to be converted to radians.
 * @return PI * degrees / 180
 */
template <std::floating_point Number = float>
constexpr Number radians(Number degrees) {
    return PI_V<Number> * degrees / 180.0;
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

template <std::floating_point Number>
struct PolarToCartesianResult {
    Number x, y;
};

template <std::floating_point Number>
struct CartesianToPolarResult {
    Number radius, angle;
};

};  // namespace detail

/**
 * @brief Converts radius and angle into x and y in 
 * the cartesian coordinate system
 *
 * @tparam Number numeric type
 * @param radius polar radius
 * @param angle polar angle
 * @return PolarToCartesianResult<Number> structure with .x and .y
 */
template <std::floating_point Number>
inline constexpr detail::PolarToCartesianResult<Number>
polarToCartesian(Number radius, Number angle) {
    return {.x = radius * std::cos(angle), .y = radius * std::sin(angle)};
}

/**
 * @brief  Converts x and y into radius and angle in 
 * the polar coordinate system
 *
 * @tparam Number numeric type
 * @param x cartesian x coordinate
 * @param y cartesian y coordinate
 * @return CartesianToPolarResult<Number> structure with 
 * .radius and .angle members
 */
template <std::floating_point Number>
inline constexpr detail::CartesianToPolarResult<Number>
cartesianToPolar(Number x, Number y) {
    return {.radius = std::hypot(x, y), .angle = std::atan(x / y)};
}

/**
 * @brief Clamps value in range (from, to)
 *
 * @tparam Number any arithmetic type
 * @param n number to clamp
 * @param from lower bound
 * @param to higher bound
 * @return clamped value
 */
template <std::floating_point Number = float>
constexpr Number clamp(Number n, Number from, Number to) {
    return n > from ? from
           : n < to ? to
                    : n;
}

/**
 * @brief Re-maps a number from one range to another.
 * @note P5.js map
 *
 * @tparam Number any arithmetic type
 * @param n the incoming value to be converted
 * @param start1 lower bound of the value's current range
 * @param stop1 upper bound of the value's current range
 * @param start2 lower bound of the value's target range
 * @param stop2 upper bound of the value's target range
 * @param withinBounds constrain the value to the newly mapped range
 * @return Remapped number
 */
template <std::floating_point Number = float>
constexpr Number map(Number n,
                     Number start1, Number stop1,
                     Number start2, Number stop2,
                     bool withinBounds = false) {
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
 * @note glsl mix
 *
 * @tparam Number any arithmetic type
 * @param x The start of the range in which to interpolate.
 * @param y The end of the range in which to interpolate.
 * @param t The value to use to interpolate between x and y.
 * @return Interpolated value
 */
template <std::floating_point Number = float>
constexpr Number mix(Number x, Number y, Number t) {
    return x * (1 - t) + y * t;
}

/**
 * @brief Linearly interpolate between two values (mix alias)
 *
 * @tparam Number any arithmetic type
 * @param x The start of the range in which to interpolate.
 * @param y The end of the range in which to interpolate.
 * @param t The value to use to interpolate between x and y.
 * @return Interpolated value
 */
template <std::floating_point Number>
constexpr Number lerp(Number x, Number y, Number t) {
    return x * (1 - t) + y * t;
}

/**
 * @brief Perform Hermite interpolation between two values
 * @note glsl smoothstep
 *
 * @tparam Number any arithmetic type
 * @param edge0 The value of the lower edge of the Hermite function.
 * @param edge1 The value of the upper edge of the Hermite function.
 * @param x The source value for interpolation.
 * @return Interpolated value
 */
template <std::floating_point Number = float>
constexpr Number smoothstep(Number edge0, Number edge1,
                            Number x) {
    constexpr Number t = clamp((x - edge0) / (edge1 - edge0), 0.0, 1.0);
    return t * t * (3.0 - 2.0 * t);
}

/**
 * @brief Generates a step function by comparing x to edge. (aka saturation)
 * @note glsl step
 *
 * @tparam Number any arithmetic type
 * @param edge The location of the edge of the step function.
 * @param x Specified value.
 * @return 0.0 if x < edge, and 1.0 otherwise.
 */
template <std::floating_point Number>
constexpr Number step(Number edge, Number x) {
    return x < edge ? 0 : 1;
}

/**
 * @brief Clamps the specified value within the range of 0.0 to 1.0
 * @note glsl saturate
 *
 * @tparam Number any arithmetic type
 * @param edge The location of the edge of the step function.
 * @param x Specified value.
 * @return clamped value
 */
template <std::floating_point Number = float>
constexpr Number saturate(Number x) {
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
constexpr float qrsqrt(float x) noexcept {
    static_assert(std::numeric_limits<float>::is_iec559);  
    // (enable only on IEEE 754)

    float y = std::bit_cast<float>(
        0x5f3759df - (std::bit_cast<std::uint32_t>(x) >> 1));

    // constexpr double y = std::bit_cast<double>(
    //     0x5fe6eb50c7aa19f9 - (std::bit_cast<std::uint64_t>(x) >> 1));

    return y * (1.5f - (x * 0.5f * y * y));
}

/**
 * @brief Computes inverse sqrt using std library, 
 * effectively performs 1 / std::sqrt(x)
 *
 * @param x number to compute inverse square root from
 * @return inv sqrt value
 */
template <class Number = float>
constexpr Number rsqrt(Number x) { return 1.0 / std::sqrt(x); }

/**
 * @brief Generates pseudorandom number inclusively between low and high.
 * Uses high presiosion system time as seed and uniform distribution
 *
 * @tparam Number target numeric type
 *
 * @return Pseudorandom number on closed range [low, high].
 */
template <std::floating_point Number = float>
Number random(Number low = 0, Number high = 1) {
    // avoiding boilerplate)
    using clock = std::chrono::high_resolution_clock;

    // seeding from long time
    static uint_fast32_t seed = clock::now().time_since_epoch().count();
    // seeding generator
    static std::default_random_engine eng(seed);

    // determine what number type was passed
    // and using appropriate distribution
    if constexpr (std::is_integral_v<Number>) {
        std::uniform_int_distribution<Number> distrib(low, high);
        return distrib(eng);
    } else {
        std::uniform_real_distribution<Number> distrib(low, high);
        return distrib(eng);
    }
}

/**
 * @brief Calculates factorial of integral number
 *
 * @tparam Number target integral type
 * @param n number to take factorial from
 * @return Factorial of number n
 */
template <std::integral Number>
constexpr Number fact(Number n) {
    if (!n) return Number{1};

    auto result = n;
    while (--n) result *= n;

    return result;
}

/**
 * @brief Computes fractional part of the number
 * @note glsl fract
 *
 * @tparam Number any floating point number type
 * @param n number to take fractional part from
 * @return The fractional part of x. This is calculated as x - floor(x).
 */
template <std::floating_point Number = float>
constexpr Number fract(Number n) { return std::modf(n, nullptr); }

/**
 * @brief Computes a modulo b
 * @see glsl mod
 *
 * @tparam Number any floating point number type
 * @param a first number
 * @param b second number
 * @return The value of a modulo b. This is computed as a - b * floor(a / b).
 */
template <std::floating_point Number = float>
constexpr Number mod(Number a, Number b) { return a - b * std::floor(a / b); }

/**
 * @brief Performs near same check of floating point number
 * @note http://realtimecollisiondetection.net/blog/?p=89
 *
 * @tparam Number any floating point number type
 * @param a first number
 * @param b second number
 * @param epsilon difference to check for 
 * std::numeric_limits<Number>::epsilon by default
 * @return true if difference between numbers less than epsilon
 */
template <std::floating_point Number = float>
constexpr bool same(Number a, Number b,
                    Number epsilon = std::numeric_limits<Number>::epsilon()) {
    constexpr Number zero = 0.0;
    constexpr Number one = 1.0;
    // assume small positive epsilon
    assert(epsilon >= zero and epsilon <= one);

    const Number diff = std::abs(a - b);
    const Number maxAB = std::max(std::abs(a), std::abs(b));

    // if the multiply won't underflow then use a multiply
    if (maxAB >= one) {
        return diff <= (epsilon * maxAB);
    }
    // multiply could underflow so use a divide if nonzero denominator
    if (maxAB > zero) {
        // correctly returns false on divide overflow
        // (inf <= epsilon is false), since overflow means the
        // relative difference is large and they are therefore not close
        return diff / maxAB <= epsilon;
    }
    // both a and b are zero
    return true;
}

/**
 * @brief Performs near same check of floating point number
 * @note http://realtimecollisiondetection.net/blog/?p=89
 *
 * @tparam Number any floating point number type
 * @param a first number
 * @param b second number
 * @param absTol absolute tolerance
 * @param relTol relative tolerance
 * @return true if difference between numbers less than epsilon
 */
template <std::floating_point Number = float>
constexpr bool same(Number a, Number b,
                    Number absTol, Number relTol) {
    /**
     * from http://realtimecollisiondetection.net/blog/?p=89
     * ...The absolute tolerance test fails when a and b become large,
     * and the relative tolerance test fails when they become small.
     * It is therefore desired to combine these two tests 
     * together in a single test...
     */
    constexpr Number diff = std::abs(a - b);
    constexpr Number maxAB = std::max(std::abs(a), std::abs(b));
    return diff <= std::max(absTol, relTol * maxAB);
}

/**
 * @brief Extracts the sign of the parameter
 * @note glsl sign
 *
 * @tparam Number any signed number type
 * @param x the value from which to extract the sign.
 * @return -1.0 if x is less than 0.0,
 *          0.0 if x is equal to 0.0,
 *         +1.0 if x is greater than 0.0.
 */
template <my::arithmetic Number = float>
constexpr Number sign(Number x) {
    if (x < 0) return -1;
    if (x > 0) return +1;
    return 0;
}

/**
 * @brief Returns sine random of seed number, can't be used as generator
 *
 * @tparam Number any floating point number type
 * @param n seeder
 * @return random value computed by formula fract(sin(n) * 43758.5453123)
 */
template <std::floating_point Number = float>
constexpr Number sinrand(Number n) {
    return fract<Number>(std::sin(n) * 43758.5453123);
}

/**
 * @brief Returns one dimensional noise value
 *
 * @tparam Number any floating point number type
 * @param p seeder
 * @return noised value
 */
template <std::floating_point Number = float>
constexpr Number noise(Number p) {
    const Number fl = std::floor(p);
    const Number fc = p - fl;  // my::fract(p)
    return mix<Number>(sinrand<Number>(fl), sinrand<Number>(fl + 1.0), fc);
}

/**
 * @brief Returns jittered value of n
 *
 * @tparam Number any floating point number type
 * @param n number to jit
 * @param j jitter force
 * @return jittered value
 */
template <std::floating_point Number = float>
constexpr Number jitter(Number n, Number j) {
    return n + j * my::random<Number>();
}

/**
 * @brief Sine cardinal function
 * @see https://mathworld.wolfram.com/SincFunction.html
 *
 * @tparam Number any floating point number type
 * @param x value in which compute sinc
 * @param k amount of 'bounces'
 * @return a phase shifted sin(x) value
 */
template <std::floating_point Number = float>
constexpr Number sinc(Number x, Number k = 1.0) {
    if (x == 0 or k == 0) return 1;
    const Number a = k * PI_V<Number> * x;
    return std::sin(a) / a;
}

/**
 * @brief Computes rectangular function
 *
 * @tparam Number any floating point number type
 * @param x value to compute
 * @return rectangular function value at x
 */
template <std::floating_point Number = float>
constexpr Number rect(Number x) {
    constexpr Number half = 0.5;
    if (std::abs(x) < half) return 1.0;
    if (std::abs(x) > half) return 0.0;
    return half;
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

}  // namespace my
#endif  // MY_MATH_HPP