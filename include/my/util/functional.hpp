#pragma once

#include <my/util/concepts.hpp>
//
#include <functional>

// function objects and lambda generators

namespace my {

/**
 * @brief Makes lambda from any callable
 *
 * @tparam Function callable
 */
template <class Function>
struct lambdaFrom {
   private:
    using Func = my::make_member_function_t<Function>;

   public:
    constexpr explicit lambdaFrom(Function function) noexcept
        : _func(std::move(function)) {
    }

    template <class... Args>
        requires std::is_invocable_v<Func, Args...>
    constexpr decltype(auto)
    operator()(Args &&...args) noexcept(
        std::is_nothrow_invocable_v<Func, Args...>) {
        return std::invoke(_func, std::forward<Args>(args)...);
    }

   private:
    Func _func;
};

/**
 * @brief
 *
 * @tparam Identity
 * @tparam Function
 */
template <class Identity, my::value Function>
struct foldWith {
   private:
    using Func = my::make_member_function_t<Function>;

   public:
    constexpr foldWith(Identity identity, Function func)
        : _identity(std::move(identity)),
          _func(std::move(func)) {}

    template <std::common_with<Identity>... Args>
    constexpr auto
    operator()(Args &&...args) const {
        auto accum = _identity;
        return ((accum = std::invoke(_func, std::move(accum),
                                     std::forward<Args>(args))),
                ...);
    }

   private:
    Identity _identity;
    Func _func;
};

/**
 * @brief
 *
 * @tparam Function
 */
template <my::value Function>
struct reducerFrom {
   private:
    using Func = my::make_member_function_t<Function>;

   public:
    constexpr explicit reducerFrom(Function func)
        : _func(std::move(func)) {}

    template <class Accum, class... Args>
        requires std::invocable<Func, std::add_lvalue_reference_t<Accum>,
                                Args...>
    constexpr void
    operator()(Accum &accum, Args &&...args) const {
        (std::invoke(_func, accum, std::forward<Args>(args)), ...);
    }

   private:
    Func _func;
};

/**
 * @brief
 *
 */
template <my::arithmetic Result = double>
struct average {
    constexpr Result
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
struct isDivisibleBy {
    constexpr explicit isDivisibleBy(Number number)
        : _number(number) {}

    constexpr bool
    operator()(std::convertible_to<Number> auto e) const noexcept {
        return e % _number == 0;
    }

   private:
    Number _number;
};

/**
 * @brief
 *
 * @tparam T
 */
template <class T>
struct equalToValue {
    constexpr explicit equalToValue(T value)
        : _value(std::move(value)) {}

    template <std::equality_comparable_with<T> U>
    constexpr bool
    operator()(U &&e) const noexcept {
        return std::forward<U>(e) == _value;
    }

   private:
    T _value;
};

namespace predicates {

/**
 * @brief Negate predicate functor generator
 *
 * @tparam Predicate functor
 */
template <my::value Predicate>
struct negate {
   private:
    using Pred = my::make_member_function_t<Predicate>;

   public:
    constexpr explicit negate(Predicate predicate)
        : _predicate(std::move(predicate)) {
    }

    template <class... Args>
        requires std::predicate<Predicate, Args...>
    constexpr bool
    operator()(Args &&...args) const noexcept {
        return not std::invoke(_predicate, std::forward<Args>(args)...);
    }

   private:
    Pred _predicate;
};

template <my::value PredicateA,
          my::value PredicateB,
          my::value Composer>
struct compose {
   private:
    using PredA = my::make_member_function_t<PredicateA>;
    using PredB = my::make_member_function_t<PredicateB>;
    using Comp = my::make_member_function_t<Composer>;

   public:
    constexpr explicit compose(PredicateA lhs,
                               PredicateB rhs,
                               Composer comp)
        : _lhs(std::move(lhs)),
          _rhs(std::move(rhs)),
          _comp(std::move(comp)) {
    }

    template <class... Args>
        requires std::predicate<PredicateA, Args...> and
                 std::predicate<PredicateB, Args...>
    constexpr bool
    operator()(Args &&...args) const noexcept {
        return _comp(std::invoke(_lhs, args...),
                     std::invoke(_rhs, args...));
    }

   private:
    PredA _lhs;
    PredB _rhs;
    Comp _comp;
};

template <my::value PredA, my::value PredB>
auto _and(PredA lhs, PredA rhs) {
    return compose(lhs, rhs, std::logical_and<void>{});
}

template <my::value PredA, my::value PredB>
auto _nand(PredA lhs, PredA rhs) { return negate(_and(lhs, rhs)); }

template <my::value PredA, my::value PredB>
auto _or(PredA lhs, PredA rhs) {
    return compose(lhs, rhs, std::logical_or<void>{});
}

template <my::value PredA, my::value PredB>
auto _nor(PredA lhs, PredA rhs) { return negate(_or(lhs, rhs)); }

template <class P>
struct Predicate {
    constexpr explicit Predicate(P p) noexcept : _p(std::move(p)) {}

    template <class... Args>
        requires std::invocable<P, Args...>
    constexpr auto operator()(Args &&...args) const -> bool {
        return std::invoke(_p, std::forward<Args>(args)...);
    }

    constexpr auto operator not() const {
        auto result = [this]<class... Args>
            requires std::invocable<P, Args...>
        (Args &&...args) -> bool {
            return not ((*this)(std::forward<Args>(args)...));
        };
        return Predicate<decltype(result)>(std::move(result));
    }

    template <class Other>
    constexpr auto operator and(Other &&other) const {
        auto result = [this, other = std::forward<Other>(other)]<class... Args>
            requires std::invocable<P, Args...> and
                         std::invocable<Other, Args...>
        (Args &&...args) -> bool {
            return (*this)(std::forward<Args>(args)...) and
                   std::invoke(other, std::forward<Args>(args)...);
        };
        return Predicate<decltype(result)>(std::move(result));
    }

    template <class Other>
    constexpr auto operator or(Other &&other) const {
        auto result = [this, other = std::forward<Other>(other)]<class... Args>
            requires std::invocable<P, Args...> and
                         std::invocable<Other, Args...>
        (Args &&...args) -> bool {
            return (*this)(std::forward<Args>(args)...) or
                   std::invoke(other, std::forward<Args>(args)...);
        };
        return Predicate<decltype(result)>(std::move(result));
    }

   private:
    P _p;
};

};  // namespace predicates

template <class F, class P, class... Args>
concept invocable_with_projection =
    std::is_invocable_v<F, std::invoke_result_t<P, Args>...>;

/**
 * @brief Projector lambda generator. Useful when you need to get member
 * value from an object.
 * In detail general mechanism of this generator is the following:
 * For each functor pointer will be added if a functor is a function.
 * project declares variadic operator()() in which
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
    using Func = my::make_member_function_t<Functor>;
    using Proj = my::make_member_function_t<Projection>;

   public:
    constexpr explicit project(Functor func, Projection proj)
        : _func(std::move(func)), proj_(std::move(proj)) {
    }

    template <class... Args>
        requires invocable_with_projection<Func, Proj, Args...>
    constexpr decltype(auto)
    operator()(Args &&...args) const noexcept {
        return std::invoke(_func, std::invoke(proj_,
                                              std::forward<Args>(args))...);
    }

   private:
    Func _func;
    Proj proj_;
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
template <my::value BiPredicate = std::less<void>,
          my::value Projection = std::identity>
struct compare {
   private:
    using Pred = my::make_member_function_t<BiPredicate>;
    using Proj = my::make_member_function_t<Projection>;

   public:
    /**
     * @brief Constructs comparator with
     * comparison function(std::less by default)
     * and projection function (std::identity by default)
     *
     */
    constexpr explicit compare(BiPredicate f = {},
                               Projection p = {}) noexcept
        : _pred(std::move(f)), _proj(std::move(p)) {
    }

    template <class T, class U>
        requires my::invocable_with_projection<BiPredicate, Projection, T, U>
    constexpr bool
    operator()(const T &a, const U &b) const {
        return std::invoke(_pred,
                           std::invoke(_proj, a),
                           std::invoke(_proj, b));
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
    constexpr auto then(BiPred2 f, Proj2 p = {}) const {
        // clang-format off
        return my::compare(
            [this, f, p]<class T, class U>
            requires my::invocable_with_projection<BiPred2, Proj2, T, U>
            (const T &a, const U &b) -> bool {
                if ((*this)(a, b)) return true;
                if ((*this)(b, a)) return false;

                // a == b for primary condition, go to secondary
                return std::invoke(f, std::invoke(p, a),
                                      std::invoke(p, b));
            });
        // clang-format on
    }

    /**
     * @brief Appends secondary projection for less comparison.
     * If primary condition evaluates to a == b, then a < b with
     * provided projection will be called
     *
     * @param p secondary projection for std::less function
     * @return compare structure that wraps lambda call
     */
    template <my::value Proj2>
    constexpr auto thenProject(Proj2 p) const {
        return then(std::less<void>{}, std::move(p));
    }

   private:
    Pred _pred;
    Proj _proj;
};

/**
 * @brief Constructs less comparator with projection
 *
 */
template <my::value Projection = std::identity>
constexpr auto compareProject(Projection p = {}) noexcept {
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
struct overload : Ts... {
    using Ts::operator()...;
};
template <class... Ts>
overload(Ts...) -> overload<Ts...>;

// clang-format off
/**
 * @brief Calls vararg amount of similar by signature functions in subsection.
 * Can be used as a callback joiner e.g. to join multiple consumer-like callbacks
 * into one.
 *
 * @tparam Funcs Similar by signature function types
 * @param funcs Similar by signature functions
 * @return anonymous lambda, that joins logic of all passed functions
 */
template <class... Funcs>
consteval auto conflate(Funcs &&...funcs) {
    return [...funcs = std::forward<Funcs>(funcs)]<class... Args>
        (Args && ...args) requires(std::invocable<Funcs, Args...> and ...) {
            (std::invoke(funcs, std::forward<Args>(args)...), ...);
        };
}
// clang-format on

}  // namespace my
