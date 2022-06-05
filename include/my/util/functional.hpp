#pragma once
#ifndef MY_FUNCTIONAL_HPP
#define MY_FUNCTIONAL_HPP

#include <functional>
#include <my/util/traits.hpp>

// function objects and lambda generators

namespace my {

/**
 * @brief Makes lambda from any callable
 *
 * @tparam Function callable
 */
template <class Function>
struct lambda_from {
   private:
    using Func_ = my::make_member_function_t<Function>;

   public:
    inline constexpr explicit lambda_from(Function function) noexcept
        : f_(std::move(function)) {
    }

    template <class... Args>
    requires std::is_invocable_v<Func_, Args...>
    inline constexpr decltype(auto)
    operator()(Args &&...args) noexcept(
        std::is_nothrow_invocable_v<Func_, Args...>) {
        return std::invoke(f_, std::forward<Args>(args)...);
    }

   private:
    Func_ f_;
};

/**
 * @brief
 *
 * @tparam Identity
 * @tparam Function
 */
template <class Identity, my::value Function>
struct fold_with {
    inline constexpr fold_with(Identity identity, Function func)
        : identity_(std::move(identity)),
          func_(std::move(func)) {}

    template <std::common_with<Identity>... Args>
    inline constexpr auto
    operator()(Args &&...args) const {
        auto accum = identity_;
        return ((accum = std::invoke(func_, std::move(accum),
                                     std::forward<Args>(args))),
                ...);
    }

   private:
    Identity identity_;
    my::make_member_function_t<Function> func_;
};

/**
 * @brief
 *
 * @tparam Function
 */
template <my::value Function>
struct reducer_from {
   private:
    using Func_ = my::make_member_function_t<Function>;

   public:
    inline constexpr explicit reducer_from(Function func)
        : func_(std::move(func)) {}

    template <class Accum, class... Args>
    requires std::invocable<Func_, std::add_lvalue_reference_t<Accum>,
                            Args...>
    inline constexpr void
    operator()(Accum &accum, Args &&...args) const {
        (std::invoke(func_, accum, std::forward<Args>(args)), ...);
    }

   private:
    Func_ func_;
};

/**
 * @brief
 *
 */
template <my::arithmetic Result = double>
struct average {
    inline constexpr Result
    operator()(my::arithmetic auto... args) const noexcept {
        return (static_cast<Result>(0.0) + ... + args) /
               static_cast<Result>(sizeof...(args));
    }
};

/**
 * @brief
 *
 * @tparam Number
 */
template <std::integral Number>
struct is_divisible_by {
    inline constexpr explicit is_divisible_by(Number n)
        : n_(n) {}

    inline constexpr bool
    operator()(std::convertible_to<Number> auto e) const noexcept {
        return e % n_ == 0;
    }

   private:
    Number n_;
};

/**
 * @brief
 *
 * @tparam T
 */
template <class T>
struct equal_to_value {
    inline constexpr explicit equal_to_value(T value)
        : value_(std::move(value)) {}

    template <std::equality_comparable_with<T> U>
    inline constexpr bool
    operator()(U &&e) const noexcept {
        return std::forward<U>(e) == value_;
    }

    T value_;
};

/**
 * @brief Negate predicate functor geenrator
 *
 * @tparam Predicate functor
 */
template <my::value Predicate>
struct negate {
    inline constexpr explicit negate(Predicate predicate)
        : predicate_(std::move(predicate)) {
    }

    template <class... Args>
    requires std::invocable<Predicate, Args...>
    inline constexpr bool operator()(Args &&...args) const noexcept {
        return not std::invoke(predicate_, std::forward<Args>(args)...);
    }

   private:
    my::make_member_function_t<Predicate> predicate_;
};

template <class F, class P, class... Args>
concept invocable_with_projection =
    std::is_invocable_v<F, std::invoke_result_t<P, Args>...>;

/**
 * @brief Projector lambda generator. Useful when you need to get member
 * value from an object.
 * In detail general mechanism of this generator is the following:
 * For each functor pointer will be added if a functor is a function.
 * project declares variadic opearator()() in which
 * projection will be applied for each received argument
 * and then passed into destination functor.
 *
 * @note Usage example:
 *  struct Person {
 *      std::string name;
 *      std::string surname;
 *      uint16_t age;
 *  };
 *
 *  std::vector<Person> persons{ ... };
 *  std::sort(begin(persons), end(persons), project(my::less{}, &Person::age));
 *
 *
 * @tparam Functor destination functor
 * @tparam Projection projection functor
 */
template <my::value Functor, my::value Projection>
struct project {
   private:
    using Func_ = my::make_member_function_t<Functor>;
    using Proj_ = my::make_member_function_t<Projection>;

   public:
    inline constexpr explicit project(Functor func, Projection proj)
        : func_(std::move(func)), proj_(std::move(proj)) {
    }

    template <class... Args>
    requires invocable_with_projection<Func_, Proj_, Args...>
    inline constexpr decltype(auto)
    operator()(Args &&...args) const noexcept {
        return std::invoke(func_, std::invoke(proj_,
                                              std::forward<Args>(args))...);
    }

   private:
    Func_ func_;
    Proj_ proj_;
};

/**
 * @brief Comparator generator with projection.
 * Accepts BiPredicates as comparison functions and applies projection for
 * received arguments.
 * Has method then() to provide secondary comparison operator with projection
 * or method thenProject() to provide only projection on less function.
 *
 * @note Usage example:
 *  struct Person {
 *      std::string name;
 *      std::string surname;
 *      uint16_t age;
 *  };
 *
 *  std::vector<Person> persons{ ... };
 *  std::ranges::sort(persons,
 *      compare(std::less<void>{}, &Person::age)
 *      .thenProject(&Person::surname)  // here std::less will be used
 *      .then(std::greater<void>{}, &Person::name));
 *
 *
 * @tparam BiPred Binary Predicate
 * @tparam Proj Projection function or field defaults down to std::identity
 */
template <my::value BiPred = std::less<void>,
          my::value Proj = std::identity>
struct compare {
   private:
    using Pred_ = my::make_member_function_t<BiPred>;
    using Proj_ = my::make_member_function_t<Proj>;

   public:
    /**
     * @brief Constructs comparator with
     * comparison function(std::less by default)
     * and projection function (std::identity by default)
     *
     */
    inline constexpr explicit compare(BiPred f = {}, Proj p = {}) noexcept
        : f_(std::move(f)), p_(std::move(p)) {
    }

    template <class T, class U>
    requires my::invocable_with_projection<Pred_, Proj_, T, U>
    inline constexpr bool
    operator()(const T &a, const U &b) const {
        return std::invoke(f_, std::invoke(p_, a), std::invoke(p_, b));
    }

    /**
     * @brief Appends secondary comparison function with projection.
     * If primary condition evaluates to f(a, b) == false
     * and f(b, a) == false then secondary condition will be called
     *
     * @param f secondary comparison function
     * @param p secondary projection function
     * @return compare structure that wraps lambda call
     */
    template <my::value BiPred2 = std::less<void>,
              my::value Proj2 = std::identity>
    inline constexpr auto then(BiPred2 f, Proj2 p = {}) const {
        // clang-format off
        return my::compare(
            [this, f, p]<class T, class U> 
            requires my::invocable_with_projection<BiPred2, Proj2, T, U>
            (const T &a, const U &b) -> bool {
                if (operator()(a, b)) return true;
                if (operator()(b, a)) return false;

                // a == b for primary condition, go to secondary
                return std::invoke(f, std::invoke(p, a), 
                                      std::invoke(p, b));
            });
        // clang-format on
    }

    /**
     * @brief Appends secondary projection for less comparison.
     * If pripary condition evaluates to a == b, then a < b with
     * provided projection will be called
     *
     * @param p secondary projection for std::less function
     * @return compare structure that wraps lambda call
     */
    template <my::value Proj2>
    inline constexpr auto thenProject(Proj2 p) const {
        return then(std::less{}, std::move(p));
    }

   private:
    Pred_ f_;
    Proj_ p_;
};

/**
 * @brief Constructs less comparator with projection
 *
 */
template <class Projection = std::identity>
inline constexpr auto compareProject(Projection p = {}) noexcept {
    return compare(std::less<void>{}, std::move(p));
}

/**
 * @brief Overload resolution entity. Useful for visiting std::variant
 *
 *  @note Usage example:
 *      std::variant<int, float> intFloat { 0.0f };
 *      std::visit(overload(
 *          [](const int& i) { ... },
 *          [](const float& f) { ... },
 *      ),
 *      intFloat;
 * );
 *
 *
 * @tparam Ts
 */
template <class... Ts>
struct overload : Ts... { using Ts::operator()...; };
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

// for my::algorithms

/**
 * @brief
 *
 */
class back_inserter {
   public:
    template <my::iterable Container, std::common_with<
                                          my::value_t<Container>>
                                          T>
    constexpr void operator()(Container &container, T &&val) const {
        container.push_back(std::forward<T>(val));
    }
};

/**
 * @brief
 *
 */
class front_inserter {
   public:
    template <my::iterable Container, std::common_with<
                                          my::value_t<Container>>
                                          T>
    constexpr void operator()(Container &container, T &&val) const {
        container.push_front(std::forward<T>(val));
    }
};

/**
 * @brief
 *
 */
class inserter {
   public:
    template <my::iterable Container, std::common_with<
                                          my::value_t<Container>>
                                          T>
    constexpr void operator()(Container &container, T &&val) const {
        container.insert(std::forward<T>(val));
    }
};

// UB prune
/**
 * @brief
 *
 */
class array_inserter {
   public:
    constexpr explicit array_inserter(size_t start = 0, size_t step = 1)
        : start_(start), step_(step) {}

    template <my::iterable Container, std::common_with<
                                          my::value_t<Container>>
                                          T>
    constexpr auto operator()(Container &container, T &&val) {
        using std::size;
        assert(start_ < size(container));
        container[start_] = std::forward<T>(val);
        start_ += step_;
    }

   private:
    size_t start_, step_;
};

// switch gate
// push_back -> insert -> array_insert -> push_front
/**
 * @brief
 *
 * @tparam Iterable
 * @return constexpr auto
 */
template <my::iterable Iterable>
constexpr auto decide_inserter() noexcept {
    if constexpr (my::applicable_v<my::detail::call_push_back, Iterable>) {
        return my::back_inserter{};
    } else if (my::applicable_v<my::detail::call_insert, Iterable>) {
        return my::inserter{};
    } else if (my::applicable_v<my::detail::call_square_brackets, Iterable>) {
        return my::array_inserter{};
    } else if (my::applicable_v<my::detail::call_push_front, Iterable>) {
        return my::front_inserter{};
    } else {
        return;
    }
}

// type trait
template <class Iterable>
struct inserter_for {
    using type = decltype(decide_inserter<Iterable>());
};

template <class Iterable>
using inserter_for_t = typename inserter_for<Iterable>::type;

}  // namespace my

#endif  // MY_FUNCTIONAL_HPP