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
template <class T = int32_t>
// typename std::enable_if_t<(std::is_arithmetic_v<T>), bool> = true
class range {
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
        const value_type m_start;
        const value_type m_stop;
        const value_type m_step;
        value_type m_curr;

        constexpr void increment(size_t amount) noexcept {
            while (amount--) m_curr += m_step;
        }

        constexpr void decrement(size_t amount) noexcept {
            while (amount--) m_curr -= m_step;
        }

       public:
        constexpr explicit range_iterator(value_type start,
                                          value_type stop,
                                          value_type step,
                                          value_type curr) noexcept
            : m_start(start), m_stop(stop), m_step(step), m_curr(curr) {
        }

        constexpr range_iterator &operator++() noexcept {
            m_curr += m_step;
            return *this;
        }

        constexpr range_iterator operator++(int) noexcept {
            auto tmp = *this;
            m_curr += m_step;
            return tmp;
        }

        constexpr range_iterator &operator--() noexcept {
            m_curr -= m_step;
            return *this;
        }

        constexpr range_iterator operator--(int) noexcept {
            auto tmp = *this;
            m_curr -= m_step;
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
                   lhs.m_start == rhs.m_start and
                   lhs.m_stop == rhs.m_stop and
                   lhs.m_step == rhs.m_step;
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
    constexpr explicit range(value_type start,
                             value_type stop,
                             value_type step = 1) noexcept
        : m_start(start),
          m_stop(stop),
          m_step(step),
          m_amount((stop - start) / step) {
        assert(m_amount > 0 and "amount of result items in container must be positive");
    }
    /**
     * @brief Create range object with start, end and step values.
     *
     * @param stop Required. Position where to stop (not included).
     */
    constexpr explicit range(value_type stop) noexcept
        : range(0, stop) {}

    constexpr const_iterator begin() const noexcept {
        return const_iterator(m_start, m_stop, m_step, m_start);
    }
    constexpr const_iterator end() const noexcept {
        return const_iterator(m_start, m_stop, m_step, m_stop);
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
            container.reserve(static_cast<c_size_type>(m_amount));
        }

        for (value_type n = m_start; n < m_stop; n += m_step) {
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

        const auto it = begin();

        buffer << "[";

        auto n = m_start;

        for (; n < m_stop - m_step; n += m_step) {
            buffer << n << ", ";
        }

        buffer << n << "]";

        os << buffer.str();
        return os;
    }

    value_type &start() noexcept { return m_start; }
    value_type start() const noexcept { return m_start; }

    value_type &stop() noexcept { return m_stop; }
    value_type stop() const noexcept { return m_stop; }

    value_type &step() noexcept { return m_step; }
    value_type step() const noexcept { return m_step; }

    value_type operator[](size_t index) {
        value_type res = m_start;
        while (index--) {
            res += m_step;
            if (res >= m_stop) return res;
        }
        return res;
    }

    friend std::ostream &operator<<(std::ostream &os, const range &obj) {
        return obj.print(os);
    }

   private:
    const value_type m_start;
    const value_type m_stop;
    const value_type m_step;
    const size_t m_amount;
};

template <class T>
explicit range(T, T, T) -> range<T>;

}  // namespace my

#endif  // MY_RANGE_HPP
