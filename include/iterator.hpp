#ifndef ITERATOR_H
#define ITERATOR_H

#include "iterator"
#include <compare>
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
    using iterator_type     = Iterator<T>;
    using iterator_concept  = std::contiguous_iterator_tag;
    using iterator_category = std::random_access_iterator_tag;
    using value_type        = std::remove_cv_t<T>;
    using difference_type   = ptrdiff_t;
    using pointer           = T *;
    using reference         = T &;

    Iterator(T *ptr) : cur(ptr) {};
    Iterator() = default;

    Iterator(Iterator<T> &&other) noexcept : cur(other.cur) {
        other.cur = nullptr;
    }
    Iterator(const Iterator<T> &other) noexcept : cur(other.cur) {}

    Iterator<T>& operator=(const Iterator<T> &other) noexcept {
        if (*this != other) {
            cur = other.cur;
        }
        return *this;
    }
    Iterator<T>& operator=(Iterator<T> &&other) noexcept {
        if (*this != other) {
            cur       = other.cur;
            other.cur = nullptr;
        }
        return *this;
    }
    friend  T&&
    iter_move(const Iterator& i) noexcept {
        return std::move(*i.cur);
    }

     friend constexpr void
    iter_swap(const Iterator& x, const Iterator& y) noexcept {
        std::swap(*x.cur, *y.cur);
    }
    value_reference operator*() noexcept { return *cur; }

    Value_pointer operator->() const noexcept { return cur; }
    iterator_ref operator++() noexcept {
        cur++;
        return *this;
    }

    Iterator<T> operator++(int) noexcept { return Iterator<T>(cur + 1); }
    Iterator<T> operator--(int) noexcept { return Iterator<T>(cur - 1); }

    iterator_ref operator--() noexcept {
        cur--;
        return *this;
    }

    std::strong_ordering operator<=>(const Iterator<T> &other) const noexcept {
        return cur <=> other.cur;
    }

    friend bool operator==(const Iterator<T>& lhs, const Iterator<T>& rhs) noexcept {
        return lhs.cur == rhs.cur;
    }
    friend bool operator!=(const Iterator<T>& lhs, const Iterator<T>& rhs) noexcept {
        return lhs.cur != rhs.cur;
    }

    void operator+=(difference_type n) noexcept { cur += n; }

    friend difference_type operator-(Iterator<T> &left,
                                     Iterator<T> &right) noexcept {
        return left.cur - right.cur;
    }
    Iterator operator+(difference_type n) const noexcept {
        return Iterator(cur + n);
    }

    Iterator operator-(difference_type n) const noexcept {
        return Iterator(cur - n);
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
