#pragma once
#ifndef FREE_LIST_HPP
#define FREE_LIST_HPP

#include <limits>
#include <vector>

#define INVALID std::numeric_limits<size_t>::max()

namespace my {

/**
 * @brief Indexed based free list with constant time removal
 * from anywhere int the list without invalidating indices.
 *
 * @tparam T trivially constructable and destructable type.
 */
template <class T>
class FreeList {
   public:
    using value_type = typename std::vector<T>::value_type;
    using pointer = typename std::vector<T>::pointer;
    using const_pointer = typename std::vector<T>::const_pointer;
    using reference = typename std::vector<T>::reference;
    using const_reference = typename std::vector<T>::const_reference;
    using iterator = typename std::vector<T>::iterator;
    using const_iterator = typename std::vector<T>::const_iterator;
    using const_reverse_iterator = typename std::vector<T>::const_reverse_iterator;
    using reverse_iterator = typename std::vector<T>::reverse_iterator;
    using size_type = typename std::vector<T>::size_type;
    using difference_type = typename std::vector<T>::difference_type;
    using allocator_type = typename std::vector<T>::allocator_type;

    FreeList() : firstFree(INVALID) {
    }

    /**
     * @brief Construct a new Free List object from initializer list.
     *
     * @param list
     */
    FreeList(std::initializer_list<value_type> list) : firstFree(INVALID) {
        reserve(list.size());
        for (auto&& el : list) {
            FreeElement fe;
            fe.element = el;
            data.emplace_back(std::move(fe));
        }
    }

    /**
     * @brief Inserts element to the free list and retruns an index to it.
     *
     * @return Index to inserted element.
     */
    size_type insert(const_reference element) {
        if (firstFree != INVALID) {
            const size_t index = firstFree;
            firstFree = data[index].next;
            data[index].element = element;
            return index;
        }
        FreeElement el;
        el.element = element;
        data.emplace_back(std::move(el));
        return data.size() - 1;
    }

    /**
     * @brief Removes element by index from free list.
     *
     * @param index
     */
    void erase(size_type index) {
        data[index].next = firstFree;
        firstFree = index;
    }

    /**
     * @brief Clears free list.
     *
     */
    void clear() {
        data.clear();
        firstFree = INVALID;
    }

    /**
     * @brief Attempts to preallocate enough memory for specified number of elements.
     *
     * @param size number of elements required.
     */
    void reserve(size_type size) { data.reserve(size); }

    /**
     * @return The number of elements in the free list.
     */
    size_type size() const { return data.size(); }

    /**
     * @brief Checks if the free list has no elements
     *
     * @return true if the free list is empty,
     * @return false otherwise
     */
    [[nodiscard]] bool empty() const { return data.empty(); }

    /**
     * @brief Subscript access to the data contained in the free list.
     *
     * @param index The index of the element for which data should be
     * accessed.
     * @return Read/write reference to data.
     */
    reference operator[](size_type index) {
        return data[index].element;
    }

    /**
     * @brief Subscript access to the data contained in the free list.
     *
     * @param index The index of the element for which data should be
     * accessed.
     * @return Read-only(constant) reference to data.
     */
    const_reference operator[](size_type index) const {
        return data[index].element;
    }

    /**
     * @brief Prints list into output stream
     *
     */
    friend std::ostream& operator<<(std::ostream& os, const FreeList& obj) {
        auto b = obj.data.begin();
        auto e = obj.data.end();

        if (b == e) {
            os << "[]";
            return os;
        }

        os << "[";
        for (; b != e - 1; ++b) {
            os << b->element << ", ";
        }
        os << b->element << "]";

        return os;
    }

   private:
    union FreeElement {
        T element;
        size_t next;
    };
    std::vector<FreeElement> data;
    size_t firstFree;
};

template <class T>
FreeList(std::initializer_list<T>) -> FreeList<T>;

}  // namespace my

#undef INVALID

#endif  // FREE_LIST_HPP