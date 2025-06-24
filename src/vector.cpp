#include "iterator.hpp"
#include <iostream>
#include <iterator>
#include <print>
#include <type_traits>
#include <vector>
using std::cout;
using std::print;
using std::println;
namespace CompactSTL {

template <typename T> class vector {
public:
    using Pointer       = T *;
    using Reference     = T &;
    using Iterator__    = Iterator<T>;
    using RIterator__   = Reverse_iterator<T>;
    using random_access = std::random_access_iterator_tag;

    vector(size_t sz) noexcept
        : data(new T[sz * 2]), _M_begin_(data), _M_end_(data + 1),
          _M_last_(data + sz * 2) {

          };
    vector(size_t sz, T value) noexcept
        : data(new T[sz * 2]), _M_begin_(data), _M_end_(data + sz),
          _M_last_(data + sz * 2) {

        auto new_begin = data;
        for (int i = 0; i < sz; i++) {
            new_begin[i] = value;
        }
    }

    template <typename U> vector(U begin_, U end_) {
        auto sz   = std::distance(begin_, end_);
        data      = new T[sz * 2];
        _M_begin_ = data;
        _M_end_   = data + sz;
       
        _M_last_ = data + sz * 2;
        std::copy(begin_, end_, this->begin());
    }

    [[nodiscard]] size_t size() { return _M_end_ - _M_begin_; }

    [[nodiscard]] size_t size() const { return _M_end_ - _M_begin_; }
    [[nodiscard]] size_t capacity() { return _M_last_ - _M_begin_; }
    [[nodiscard]] size_t capacity() const { return _M_last_ - _M_begin_; }

    [[nodiscard]] Iterator__ begin() { return Iterator__{_M_begin_}; }
    [[nodiscard]] Iterator__ cbegin() { return Iterator__{_M_begin_}; }
    [[nodiscard]] Iterator__ end() { return Iterator__{_M_end_}; }
    [[nodiscard]] Iterator__ cend() { return Iterator__{_M_end_}; }

    [[nodiscard]] RIterator__ rbegin() { return RIterator__{_M_end_ - 1}; }
    [[nodiscard]] RIterator__ crbegin() { return RIterator__{_M_end_ - 1}; }
    [[nodiscard]] RIterator__ rend() { return RIterator__{_M_begin_ - 1}; }
    [[nodiscard]] RIterator__ crend() { return RIterator__{_M_begin_ - 1}; }

    T *operator*() { return _M_begin_; }

    void shrink_to_fit() {}

    void push_back(const T &value) {}

private:
    void _M_resize(size_t new_size) {}
    T *data;
    T *_M_begin_;
    T *_M_end_;
    T *_M_last_;
    // Iterator<T> iterator_;
    //  size_t capacity;
};

} // namespace CompactSTL
int main() {

    using namespace CompactSTL;

    std::vector<int> stdvec({1, 2, 3, 4, 5});
    CompactSTL::vector<int> vec(stdvec.begin(), stdvec.end());
    println("{}", vec.size());
    for (auto it = vec.rbegin(); it != vec.rend(); ++it) {
        println("{}", *it);
    }

    return 0;
}