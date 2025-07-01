#include <initializer_list>
#include <iostream>
#include <iterator.hpp>
#include <memory>
#include <print>
#include <stdexcept>
#include <type_traits>
namespace CompactSTL {

template <typename T, size_t N> class Array {
private:
    T _M_data[N];

public:
    using Pointer_Container   = Array<T, N> *;
    using Reference_Container = Array<T, N> &;
    using Iterator            = Iterator<T>;
    using Reverse_iterator    = Reverse_iterator<T>;
    using value_type          = T;
    using value_reference     = T &;
    using value_pointer       = T *;

    Iterator begin() { return Iterator(data()); };
    Iterator begin() const { return Iterator(data()); };
    Iterator end() { return Iterator(data() + N); };
    Iterator end() const { return Iterator(data() + N); };

    Reverse_iterator rbegin() { return Reverse_iterator(data() + N - 1); };
    Reverse_iterator rbegin() const {
        return Reverse_iterator(data() + N - 1);
    };
    Reverse_iterator rend() { return Reverse_iterator(data() - 1); };
    Reverse_iterator rend() const { return Reverse_iterator(data() - 1); };

    value_reference operator[](size_t idx) { return (_M_data[idx]); };

    value_pointer data() { return +_M_data; }

    size_t capacity() { return N; }
    size_t size() { return N; }

    bool empty() { return N == 0; }

    void fill(std::initializer_list<T> &lst) {
        std::uninitialized_copy(lst.begin(), lst.end(), begin());
    }

    value_reference at(size_t idx) {
        if (idx >= N) {
            throw std::runtime_error("index out of bound");
        }
        return _M_data[idx];
    }

    value_reference front() { return _M_data[0]; }
    value_reference back() { return _M_data[N - 1]; }

    void swap(Array<T, N> &other) noexcept {
        using std::swap;
        swap(_M_data, other._M_data);
    }
};
static_assert(std::is_trivial_v<CompactSTL::Array<int, 8>> == 1,
              "Compact Array is not trivial");
static_assert(std::is_standard_layout_v<CompactSTL::Array<int, 8>> == 1,
              "Compact Array is not standard layout");
} // namespace CompactSTL