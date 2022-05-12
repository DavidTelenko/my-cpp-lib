#pragma once
#ifndef MY_TRIE_HPP
#define MY_TRIE_HPP

#include <iostream>  // std::basic_ostream
#include <map>       // std::map
#include <memory>    // std::shared_ptr, std::enable_shared_from_this
#include <set>       // std::set
#include <string>    // std::basic_string

namespace my {

/**
 * @brief Trie data structure, useful for storing and searching strings,
 * templated nature of this container gives possibilities to store any containers inside of it
 *
 * @see https://en.wikipedia.org/wiki/Trie#:~:text=In%20computer%20science%2C%20a%20trie,key%2C%20but%20by%20individual%20characters.
 *
 * @tparam T Mainly char types
 * @tparam Container value_type Mainly string types
 */
template <class T = char, class Container = std::basic_string<T>>
class BasicTrie {
    struct TrieNode : public std::enable_shared_from_this<TrieNode> {
        using Ref = std::shared_ptr<TrieNode>;

        std::map<T, Ref> children{};
        bool terminal = false;

        TrieNode() = default;

        template <class C>
        void log(std::basic_ostream<C>& os, Container& accum) const {
            if (terminal) os << accum << ", ";

            for (auto&& el : children) {
                accum.push_back(el.first);
                el.second->log(os, accum);
                accum.pop_back();
            }
        }

        template <class Accumulator>
        void collect(Accumulator& result, Container& accum) const {
            if (terminal) result.insert(accum);

            for (auto&& el : children) {
                accum.push_back(el.first);
                el.second->collect(result, accum);
                accum.pop_back();
            }
        }

        template <class Accumulator>
        void collect(Accumulator& result,
                     Container& accum,
                     typename Container::const_iterator it,
                     typename Container::const_iterator end,
                     const T& wildcard) const {
            if (it == end) {
                if (terminal) result.insert(accum);
                collect(result, accum);
                return;
            }

            if (*it == wildcard) {
                for (auto&& el : children) {
                    accum.push_back(el.first);
                    el.second->collect(result, accum, std::next(it), end, wildcard);
                    accum.pop_back();
                }
                return;
            }

            if (auto child = children.find(*it); child != children.end()) {
                accum.push_back(child->first);
                child->second->collect(result, accum, std::next(it), end, wildcard);
                accum.pop_back();
            }
        }

        // TODO test me
        auto erase(typename Container::const_iterator it,
                   typename Container::const_iterator end,
                   bool& deleted) {
            if (it == end) {
                if (terminal) deleted = !(terminal = false);
                return this->shared_from_this();
            }

            if (children.find(*it) != children.end())
                children[*it] = children[*it]->erase(std::next(it), end, deleted);

            return this->shared_from_this();
        }
    };

    using Node = typename TrieNode::Ref;
    Node root = std::make_shared<TrieNode>();

   public:
    using value_type = Container;
    using size_type = typename Container::size_type;

    BasicTrie() = default;

    /**
     * @brief Construct a new Trie object from init list
     *
     * @param list initializer list of Container<T> objects
     */
    explicit BasicTrie(std::initializer_list<value_type> list) {
        for (auto&& el : list) insert(el);
    }

    /**
     * @brief Adds value to trie
     *
     * @param obj value to add
     * @return true if trie didn't contain given value and insertion occurred false otherwise
     */
    bool insert(const value_type& obj) {
        auto tmp = root;

        for (auto el : obj) {
            auto& children = tmp->children;

            if (children.find(el) == children.end())
                children[el] = std::make_shared<TrieNode>();

            tmp = children[el];
        }

        return tmp->terminal ? false : (tmp->terminal = true);
    }

    /**
     * @brief Checks wether trie contains provided value
     *
     * @param obj value to search for
     * @return true if trie contains given value false otherwise
     */
    bool contains(const value_type& obj) const {
        auto tmp = root;

        for (auto el : obj) {
            auto& children = tmp->children;

            if (children.find(el) == children.end())
                return false;

            tmp = children[el];
        }

        return tmp->terminal;
    }

    /**
     * @brief Removes one element that equal to obj
     *
     * @param obj value to remove
     * @return true if removal occurred false otherwise
     */
    bool erase(const value_type& obj) {
        bool result = false;
        root = root->erase(std::begin(obj), std::end(obj), result);
        return result;
    }

    /**
     * @brief Finds all elements which starts with prefix.
     *
     * @tparam ResultContainer container of elements of type container_type<value_type>
     * @param prefix Container of starting values
     * @return container of elements starting with prefix
     */
    template <class ResultContainer = std::set<value_type>>
    ResultContainer collect(value_type prefix) const {
        ResultContainer result{};
        if (prefix.empty()) return result;

        auto tmp = root;

        for (auto&& el : prefix) {
            auto& children = tmp->children;

            if (children.find(el) == children.end())
                return result;

            tmp = children[el];
        }

        tmp->collect(result, prefix);  // here prefix passed by reference!
        return result;
    }

    /**
     * @brief Finds all elements which starts with (matches) prefix.
     * Uses wildcards to accept any element at given position
     *
     * @tparam ResultContainer container of elements of type container_type<value_type>
     * @param prefix Container of starting values
     * @param wildcard marker element to accept any element at given place
     * @return container of elements starting with / matching prefix
     */
    template <class ResultContainer = std::set<value_type>>
    ResultContainer collect(const value_type& prefix, const T& wildcard) const {
        ResultContainer result{};

        if (prefix.empty()) return result;

        Container accum;
        root->collect(result, accum, std::begin(prefix), std::end(prefix), wildcard);

        return result;
    }

    /**
     * @brief Check wethere trie is empty
     *
     */
    bool empty() const { return root->children.empty(); }

    /**
     * @brief Clears trie
     *
     */
    void clear() { root.reset(new TrieNode()); }

    /* printing routines */

    /**
     * @brief Prints data into basic_ostream<CharT> (std::cout by default)
     *
     * @tparam CharT char type (char by default)
     * @param os ostream to print to
     * @return ostream& to os
     */
    template <class CharT = char>
    auto& print(std::basic_ostream<CharT>& os = std::cout) const {
        if (empty()) {
            os << "[]";
            return os;
        }

        Container accum{};

        os << "[";
        root->log(os, accum);
        os << "\b\b  \b\b]";

        return os;
    }

    /**
     * @brief Prints data into basic_stringstream<CharT> then returns stream.str()
     *
     * @tparam CharT char type (char by default)
     * @return basic_string<CharT>
     */
    template <class CharT = char>
    auto toString() const {
        std::basic_stringstream<CharT> ss;
        return print(ss).str();
    }

    /**
     * @brief Prints data into basic_ostream<CharT> (std::cout by default) then prints newline
     *
     * @tparam CharT char type (char by default)
     * @param os ostream to print to
     * @return ostream& to os
     */
    template <class CharT = char>
    auto& log(std::basic_ostream<CharT>& os = std::cout) const {
        print(os) << os.widen('\n');
        return os;
    }

    template <class CharT>
    friend std::basic_ostream<CharT>&
    operator<<(std::basic_ostream<CharT>& os, const BasicTrie& obj) {
        return print(os);
    }
};

using Trie = BasicTrie<char>;
using WTrie = BasicTrie<wchar_t>;
using U16Trie = BasicTrie<char16_t>;
using U32Trie = BasicTrie<char32_t>;

}  // namespace my

#endif  // MY_TRIE_HPP