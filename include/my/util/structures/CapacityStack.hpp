#pragma once
#ifndef MY_CAPACITY_STACK_HPP
#define MY_CAPACITY_STACK_HPP

#include <array>

#ifndef LOOSE_CAPACITY_STACK
#define ERR_TOP()   \
    if (not m_size) \
        throw std::runtime_error("trying to get top from empty CapacityStack");
#define ERR_POP()   \
    if (not m_size) \
        throw std::runtime_error("trying to pop from empty CapacityStack");
#else
#define ERR_TOP()  // this may lead to unpredictable results
#define ERR_POP()
#endif

namespace my {

/**
 * @brief Array adaptor, represents stack with max reachability.
 * After reaching its capacity elements on the bottom of the stack
 * gets overriden and virtually appears at the top.
 *
 * @tparam T trivially constructable and destructable type.
 * @tparam Capacity size of reachability
 */
template <class T, size_t Capacity>
class CapacityStack {
   public:
    using value_type = T;
    using size_type = size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;

    /**
     * @brief Default constructor
     *
     */
    CapacityStack() : m_top(0), m_size(0) {}

    /**
     * @brief Accesses the top element.
     *
     * @return Read-only(Constant) reference to the rop element of the stack.
     */
    const_reference top() const {
        ERR_TOP();
        return m_stack[m_top - 1];
    }

    /**
     * @brief Accesses the top element.
     *
     * @return Read/write reference to the top element of the stack.
     */
    reference top() {
        ERR_TOP();
        return m_stack[m_top - 1];
    }

    /**
     * @brief Checks if the underlying container has no elements.
     *
     * @return true if the underlying container is empty,
     * @return false otherwise
     */
    [[nodiscard]] bool empty() const { return not m_size; }

    /**
     * @brief Returns the number of elements in the underlying container.
     *
     * @return The number of elements in the container.
     */
    constexpr size_type size() const { return m_size; }

    /**
     * @return The number of elements after witch
     * bottom element will be overriden.
     */
    constexpr size_type capacity() const { return Capacity; }

    /**
     * @brief Inserts element at the top
     *
     * @param el The value to push.
     */
    void push(const_reference el) {
        // size will be incremented only if it is less than capacity
        m_size += (m_size != Capacity);
        // if reached capacity start over
        if (m_top == Capacity) m_top = 0;
        m_stack[m_top++] = el;
    }

    /**
     * @brief Removes the top element from the stack.
     *
     */
    void pop() {
        ERR_POP();
        --m_size;
        // if reached "first" element go to the end
        if (--m_top == 1) m_top = Capacity;
    }

    /**
     * @brief Removes the top element from the stack and
     * returns reference to removed element.
     * Reference will be invalidated on the next push call!
     *
     */
    const_reference pop_get() {
        ERR_POP();

        --m_size;

        const auto tmp = m_top;
        if (--m_top == 1) m_top = Capacity;

        // here we're tricking the system and returning
        // what "supposed" to be deleted and not temporary
        return m_stack[tmp - 1];
    }

    /**
     * @brief Removes the top element from the stack 
     * and replaces with provided value.
     *
     */
    void pop_push(const_reference el) {
        ERR_POP();

        --m_size;

        const auto tmp = m_top;
        if (--m_top == 1) m_top = Capacity;
        m_stack[tmp - 1] = el;
    }

    /**
     * @brief Clears container.
     */
    void clear() {
        m_size = 0;
        m_top = 0;
    }

#ifdef ENABLE_CAPACITY_STACK_ITERATORS
    auto begin() { return m_stack.begin(); }
    auto end() { return m_stack.end(); }
    auto begin() const { return m_stack.begin(); }
    auto end() const { return m_stack.end(); }
#endif

   private:
    std::array<value_type, Capacity> m_stack;
    size_type m_top;
    size_type m_size;
};

}  // namespace my

#undef ERR_TOP
#undef ERR_POP

#endif  // MY_CAPACITY_STACK_HPP