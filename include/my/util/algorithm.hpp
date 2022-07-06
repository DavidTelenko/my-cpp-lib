#pragma once

#include <my/util/functional.hpp>
#include <my/util/traits.hpp>
//
#include <cassert>
#include <ranges>

namespace my {

// stl-like single iterator + container algorithms

/**
 * @brief Boyer-Moore majority algorithm
 * @see https://en.wikipedia.org/wiki/Boyer–Moore_majority_vote_algorithm
 *
 * @tparam It InputIterator concept
 * @param first begin of range
 * @param last end of range
 * @return It iterator pointing to the major element or to last if not found one
 */
template <std::input_iterator It,
          class BiPredicate =
              std::equal_to<typename std::iterator_traits<It>::value_type>>
constexpr It
majority(It first, It last, BiPredicate pred = BiPredicate{}) {
    if (first == last) return first;

    auto result = first;
    auto secpath = first;

    size_t count = 1;
    size_t size = 1;

    for (++first; first != last; ++first, ++size) {
        if (not count) {
            result = first;
            count = 1;
            continue;
        }
        if (std::invoke(pred, *first, *result)) {
            ++count;
            continue;
        }
        --count;
    }

    for (count = 0; secpath != last; ++secpath) {
        if (std::invoke(pred, *secpath, *result)) ++count;
    }

    return (count > size / 2) ? result : last;
}

/**
 * @brief
 *
 * @tparam Container
 * @tparam BiPredicate
 * @param container
 * @param pred
 * @return constexpr auto
 */
template <std::ranges::range Container,
          class BiPredicate = std::equal_to<my::value_t<Container>>>
constexpr auto
majority(const Container &container, BiPredicate pred = BiPredicate{}) {
    return my::majority(std::ranges::begin(container),
                        std::ranges::end(container),
                        pred);
}

// stl-like multiple iterator + container parallel algorithms

/**
 * @brief
 *
 * @tparam NaryFunction
 * @tparam InIt
 * @tparam Iterators
 * @param first
 * @param last
 * @param f
 * @param rest
 * @return my::require_input_iter<InIt, NaryFunction>
 */
template <class NaryFunction, std::input_iterator InIt,
          std::input_iterator... Iterators>
constexpr NaryFunction
forEach(InIt first, InIt last, NaryFunction f,
        Iterators... rest) {
    for (; first != last; ++first, (++rest, ...)) {
        std::invoke(f, *first, *(rest)...);
    }
    return f;
}

/**
 * @brief
 *
 * @tparam NaryFunction
 * @tparam Container
 * @tparam Containers
 * @param container
 * @param f
 * @param rest
 * @return my::require_iterable<Container, NaryFunction>
 */
template <class NaryFunction, std::ranges::range Container,
          std::ranges::range... Containers>
constexpr NaryFunction
forEach(Container &container, NaryFunction f,
        Containers &...rest) {
    return my::forEach(std::ranges::begin(container),
                       std::ranges::end(container),
                       std::move(f),
                       std::ranges::begin(rest)...);
}

/**
 * @brief
 *
 * @tparam NaryFunction
 * @tparam Container
 * @tparam Containers
 * @param f
 * @param container
 * @param rest
 * @return my::require_iterable<Container, NaryFunction>
 */
template <class NaryFunction, std::ranges::range Container,
          std::ranges::range... Containers>
constexpr NaryFunction
forEach(NaryFunction f, const Container &container,
        const Containers &...rest) {
    return my::forEach(std::ranges::cbegin(container),
                       std::ranges::cend(container),
                       std::move(f),
                       std::ranges::cbegin(rest)...);
}

template <std::input_iterator InIt, std::weakly_incrementable Out,
          class NaryOperator, std::input_iterator... Iterators>
constexpr Out
transform(InIt first, InIt last, Out result,
          NaryOperator f,
          Iterators... rest) {
    for (; first != last; ++first, ++result, (++rest, ...)) {
        *result = std::invoke(f, *first, *(rest)...);
    }
    return result;
}

template <std::ranges::range Container, std::weakly_incrementable Out,
          class NaryOperator, std::ranges::range... Containers>
constexpr Out
transform(const Container &container, Out result,
          NaryOperator f,
          const Containers &...rest) {
    return my::transform(std::ranges::begin(container),
                         std::ranges::end(container),
                         std::move(result),
                         std::move(f),
                         std::ranges::begin(rest)...);
}

/**
 * @brief
 *
 * @tparam Predicate
 * @tparam InIt
 * @tparam Iterators
 * @param first
 * @param last
 * @param pred
 * @param rest
 * @return constexpr my::require_input_iter<InIt, bool>
 */
template <class Predicate, std::input_iterator InIt,
          std::input_iterator... Iterators>
constexpr bool
any(InIt first, InIt last, Predicate pred,
    Iterators... rest) {
    for (; first != last; ++first, (++rest, ...)) {
        if (std::invoke(pred, *first, *(rest)...)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief
 *
 * @tparam Predicate
 * @tparam Container
 * @tparam Containers
 * @param container
 * @param pred
 * @param rest
 * @return constexpr my::require_iterable<Container, bool>
 */
template <class Predicate, std::ranges::range Container,
          std::ranges::range... Containers>
constexpr bool
any(Container &&container, Predicate pred,
    Containers &&...rest) {
    assert(((size(rest) >= size(container)) and ...));
    return my::any(std::ranges::cbegin(container),
                   std::ranges::cend(container),
                   pred,
                   std::ranges::cbegin(rest)...);
}

/**
 * @brief
 *
 * @tparam Predicate
 * @tparam InIt
 * @tparam Iterators
 * @param first
 * @param last
 * @param pred
 * @param rest
 * @return constexpr my::require_input_iter<InIt, bool>
 */
template <class Predicate, std::input_iterator InIt,
          std::input_iterator... Iterators>
constexpr InIt
all(InIt first, InIt last, Predicate pred,
    Iterators... rest) {
    return not my::any(std::move(first),
                       std::move(last),
                       my::negate(pred),
                       std::move(rest)...);
}

/**
 * @brief
 *
 * @tparam Predicate
 * @tparam Container
 * @tparam Containers
 * @param container
 * @param pred
 * @param rest
 * @return constexpr my::require_iterable<Container, bool>
 */
template <class Predicate, std::ranges::range Container,
          std::ranges::range... Containers>
constexpr bool
all(Container &&container, Predicate pred,
    Containers &&...rest) {
    assert(((size(rest) >= size(container)) and ...));
    return my::all(std::ranges::cbegin(container),
                   std::ranges::cend(container),
                   pred,
                   std::ranges::cbegin(rest)...);
}

/**
 * @brief
 *
 * @tparam NaryFunction
 * @tparam Accum
 * @tparam InIt
 * @tparam Iterators
 * @param first
 * @param last
 * @param f
 * @param accum
 * @param rest
 * @return my::require_input_iter<InIt, Accum>
 */
template <class NaryFunction, class Accum, std::input_iterator InIt,
          std::input_iterator... Iterators>
constexpr Accum
reduce(InIt first, InIt last, Accum accum, NaryFunction f,
       Iterators... rest) {
    for (; first != last; ++first, (++rest, ...)) {
        f(accum, *first, *(rest)...);
    }
    return accum;
}

/**
 * @brief Applyies NaryFunction for each element of range [begin(container), end(container)),
 * first element of functor needs to be reference to the accumulator value,
 * elements of subsequent ranges will be passed after the value of first range,
 * if any of the rest... containers has less elements than container it will result in UB
 *
 * @tparam NaryFunction function with the following signature
 *      (Accum& a, Container::value_type e, Containers::value_type... es)
 * @tparam Accum Accumulator type
 * @param container Iterable concept value
 * @param f NaryFunctor value
 * @param accum Accum value
 * @param rest containers to iterate in parallel (optional)
 * @return Accumulated value
 */
template <class NaryFunction, class Accum, std::ranges::range Container,
          std::ranges::range... Containers>
constexpr Accum
reduce(Container &&container, Accum accum, NaryFunction f,
       Containers &&...rest) {
    assert(((size(rest) >= size(container)) and ...));
    return my::reduce(std::ranges::cbegin(container),
                      std::ranges::cend(container),
                      std::move(accum),
                      std::move(f),
                      std::ranges::cbegin(rest)...);
}

}  // namespace my