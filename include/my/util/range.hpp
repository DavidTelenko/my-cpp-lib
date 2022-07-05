#pragma once
#ifndef MY_RANGE_HPP
#define MY_RANGE_HPP

#include <my/util/utils.hpp>
//
#include <cassert>

namespace my {

/**
 * @brief Immutable, forward, pull based range,
 * provides ability to iterate over virtual elements of range and
 * transfer values into containers.
 * Range represented in the following way: [begin, end), step
 *
 * @tparam T data type of range elements, int32_t by default, only numeric values required
 */
template <my::arithmetic T = int32_t>
// typename std::enable_if_t<(std::is_arithmetic_v<T>), bool> = true
class Range {
    struct range_iterator;

   public:
    using value_type = T;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = T *const;
    using iterator = range_iterator;
    using const_iterator = range_iterator;

   private:
    class range_iterator {
        const value_type _start;
        const value_type _stop;
        const value_type _step;
        value_type m_curr;

        constexpr void increment(size_t amount) noexcept {
            while (amount--) m_curr += _step;
        }

        constexpr void decrement(size_t amount) noexcept {
            while (amount--) m_curr -= _step;
        }

       public:
        constexpr explicit range_iterator(value_type start,
                                          value_type stop,
                                          value_type step,
                                          value_type curr) noexcept
            : _start(start), _stop(stop), _step(step), m_curr(curr) {
        }

        constexpr range_iterator &operator++() noexcept {
            m_curr += _step;
            return *this;
        }

        constexpr range_iterator operator++(int) noexcept {
            auto tmp = *this;
            m_curr += _step;
            return tmp;
        }

        constexpr range_iterator &operator--() noexcept {
            m_curr -= _step;
            return *this;
        }

        constexpr range_iterator operator--(int) noexcept {
            auto tmp = *this;
            m_curr -= _step;
            return tmp;
        }

        constexpr range_iterator &operator+(size_t i) noexcept {
            increment(i);
            return *this;
        }
        constexpr range_iterator &operator-(size_t i) noexcept {
            decrement(i);
            return *this;
        }

        constexpr value_type operator*() noexcept { return m_curr; }

        friend constexpr bool
        operator==(const range_iterator &lhs,
                   const range_iterator &rhs) noexcept {
            return lhs.m_curr == rhs.m_curr and
                   lhs._start == rhs._start and
                   lhs._stop == rhs._stop and
                   lhs._step == rhs._step;
        }
        friend constexpr bool
        operator!=(const range_iterator &lhs,
                   const range_iterator &rhs) noexcept { return !(lhs == rhs); }
    };

   public:
    /**
     * @brief Create range object with start, end and step values.
     *
     * @param start Begin of range.
     * @param stop Required. Position where to stop (not included).
     * @param step An increment value. 1 by default
     */
    constexpr explicit Range(value_type start,
                             value_type stop,
                             value_type step = 1) noexcept
        : _start(start),
          _stop(stop),
          _step(step),
          _amount((stop - start) / step) {
        assert(_amount > 0 and "amount of result items in container must be positive");
    }
    /**
     * @brief Create range object with start, end and step values.
     *
     * @param stop Required. Position where to stop (not included).
     */
    constexpr explicit Range(value_type stop) noexcept
        : Range(0, stop) {}

    constexpr const_iterator begin() const noexcept {
        return const_iterator(_start, _stop, _step, _start);
    }
    constexpr const_iterator end() const noexcept {
        return const_iterator(_start, _stop, _step, _stop);
    }

    /**
     * @brief Inserts range into provided container
     *
     * @tparam Container any container
     * @tparam InserterT inserter function witch takes reference to container and value to insert
     * @param container reference to container where to insert elements
     * @param in inserter function
     */
    template <class Container>
    constexpr void insert(Container &container) const {
        using c_val_type = typename Container::value_type;
        using c_size_type = typename Container::size_type;

        static_assert(std::is_convertible_v<c_val_type, value_type>);

        if (my::has_reserve_v<Container>) {
            container.reserve(static_cast<c_size_type>(_amount));
        }

        for (value_type n = _start; n < _stop; n += _step) {
            in(container, n);
        }

        return container;
    }

    /**
     * @brief Same as insert(), but container will be created and returned in the proccess
     *
     */
    template <class Container, class InserterT = my::inserter_for_t<Container>>
    constexpr Container convert(InserterT in = InserterT{}) const {
        Container container;
        return insert(container, in);
    }

    /**
     * @brief Prints range into ostream,
     * uses static cache to save printed data,
     * next call of the function will print this
     * cache instead of calculating values again
     *
     * @param os std output stream reference (std::cout by default)
     * @return std::ostream& reference to os
     */
    std::ostream &print(std::ostream &os = std::cout) const {
        static std::stringstream buffer;

        if (not buffer.str().empty()) {
            os << buffer.str();
            return os;
        }

        buffer << "[";

        auto n = _start;

        for (; n < _stop - _step; n += _step) {
            buffer << n << ", ";
        }

        buffer << n << "]";

        os << buffer.str();
        return os;
    }

    value_type &start() noexcept { return _start; }
    value_type start() const noexcept { return _start; }

    value_type &stop() noexcept { return _stop; }
    value_type stop() const noexcept { return _stop; }

    value_type &step() noexcept { return _step; }
    value_type step() const noexcept { return _step; }

    value_type operator[](size_t index) {
        value_type res = _start;
        while (index--) {
            res += _step;
            if (res >= _stop) return res;
        }
        return res;
    }

    friend std::ostream &operator<<(std::ostream &os, const Range &obj) {
        return obj.print(os);
    }

   private:
    const value_type _start;
    const value_type _stop;
    const value_type _step;
    const size_t _amount;
};

template <class T>
explicit Range(T, T, T) -> Range<T>;

template <my::arithmetic T>
constexpr auto range(T start, T stop, T step) {
    return Range(start, stop, step);
}

}  // namespace my

#endif  // MY_RANGE_HPP
