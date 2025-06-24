#ifndef ITERATOR_H
#define ITERATOR_H

#include "iterator"
#include <cstddef>
#include <iterator>
#include <type_traits>
namespace CompactSTL {

template <typename T, typename = void> struct is_iterator : std::false_type {};

template <typename T>
struct is_iterator<T, std::void_t<decltype(std::declval<T>().operator++()),
                                  decltype(std::declval<T>().operator*())>>
    : std::true_type {};

template <typename T> constexpr bool is_iterator_v = is_iterator<T>::value;

template <typename T> class Iterator {
private:
    T *cur;

public:
    using Pointer           = T *;
    using Reference         = Iterator<T> &;
    using iterator_category = std::random_access_iterator_tag;
    using iterator_type     = Iterator<T>;

    Iterator(T *ptr) : cur(ptr) {};

    T &operator*() { return *cur; }

    Reference operator->() { return *this; }
    Reference operator++() {
        cur++;
        return *this;
    }

    Reference operator--() {
        cur--;
        return *this;
    }

    friend bool operator==(Iterator<T> lhs, Iterator<T> rhs) {
        return lhs.cur == rhs.cur;
    }
    friend bool operator!=(Iterator<T> lhs, Iterator<T> rhs) {
        return lhs.cur != rhs.cur;
    }
};

template <typename T> class Reverse_iterator {
private:
    T *cur;

public:
    using Pointer           = T *;
    using Reference         = T &;
    using iterator_category = std::random_access_iterator_tag;
    using iterator_type     = Reverse_iterator<T>;

    Reverse_iterator(T *ptr) : cur(ptr) {};

    T operator*() { return *cur; }

    Reference operator->() { return cur; }
    Reference operator++() {
        cur--;
        return *cur;
    }

    Reference operator--() {
        cur++;
        return cur;
    }

    friend bool operator==(Reverse_iterator<T> lhs, Reverse_iterator<T> rhs) {
        return lhs.cur == rhs.cur;
    }
    friend bool operator!=(Reverse_iterator<T> lhs, Reverse_iterator<T> rhs) {
        return lhs.cur != rhs.cur;
    }
};
} // namespace CompactSTL

#endif
