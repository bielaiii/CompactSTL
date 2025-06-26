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
    using Value_pointer     = T *;
    using value_reference   = T &;
    using iterator_ref      = Iterator<T> &;
    //using iterator_category = std::random_access_iterator_tag;
    using iterator_type     = Iterator<T>;
    using iterator_concept  = std::contiguous_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = std::remove_cv_t<T>;
    using difference_type   = ptrdiff_t;
    using pointer           = T *;
    using reference         = T &;


    Iterator(T *ptr) : cur(ptr) {};
    Iterator() = default;

    value_reference operator*() { return *cur; }

    Value_pointer operator->() const { return cur; }
    iterator_ref operator++() {
        cur++;
        return *this;
    }

    iterator_ref operator--() {
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
    using value_type        = T;
    Reverse_iterator(T *ptr) : cur(ptr) {};

    Reference operator*() const { return *cur; }

    Pointer operator->() const { return cur; }
    Reference operator++() {
        cur--;
        return *cur;
    }

    Reference operator--() {
        cur++;
        return cur;
    }

    Reference operator=(int i) {
        *cur = i;
        return *cur;
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
