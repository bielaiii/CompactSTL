#include "iterator.hpp"
#include <iostream>
#include <iterator>
#include <memory>
#include <ostream>
#include <print>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>
using std::cout;
using std::print;
using std::println;
namespace CompactSTL {

template <typename T> struct VectorDetail {
    T *_M_begin_;
    T *_M_end_;
    T *_M_last_;
    VectorDetail() = default;
    VectorDetail(T *begin_, T *end_, T *last_)
        : _M_begin_(begin_), _M_end_(end_), _M_last_(last_){};
    VectorDetail(T *begin, size_t sz)
        : _M_begin_(begin), _M_end_(begin), _M_last_(begin + sz){};
    VectorDetail(size_t capacity_) noexcept {
        _M_begin_ = :: operator new (sizeof(T[capacity_]));
        _M_end_   = _M_begin_;
        _M_last_  = _M_begin_ + capacity_;
    }


    VectorDetail(VectorDetail<T> & other) {
        auto size = _M_last_ - _M_begin_;
        _M_begin_ = ::operator new(sizeof(T) * size);
        _M_last_ = _M_begin_ + size;
        _M_end_ = _M_begin_;
    }
    friend void swap(VectorDetail<T> &lhs, VectorDetail<T> &rhs) {
        using namespace std;
        swap(lhs._M_begin_, rhs._M_begin_);
        swap(lhs._M_end_, rhs._M_end_);
        swap(lhs._M_last_, rhs._M_last_);
    }
    ~VectorDetail() { delete _M_begin_; }
};

template <typename T> class vector {
private:
    void _M_resize(size_t new_size) {}
    T *src_;
    T *_M_begin_;
    T *_M_end_;
    T *_M_last_;

    void resize() {
        auto old_szie = size();
        auto new_size = old_szie * 2;

        T *new_dst    = new T[new_size];
        auto new_iter = Iterator<T>(new_dst);
        std::uninitialized_copy(begin(), end(), new_iter);

        delete[] src_;
        src_      = new_dst;
        _M_begin_ = new_dst;
        _M_end_   = _M_begin_ + old_szie;
        _M_last_  = _M_begin_ + new_size;
    }

    void safe_swap(vector<T> &vt) {

        vector<T> temp(1);
        std::swap(this->, Tp & y)
    }

public:
    using Pointer     = T *;
    using Reference   = T &;
    using Iterator__  = Iterator<T>;
    using RIterator__ = Reverse_iterator<T>;

    vector(size_t sz = 10) noexcept
        : src_(new T[sz * 2]), _M_begin_(src_), _M_end_(src_ + 1),
          _M_last_(src_ + sz * 2){

          };
    vector(size_t sz, T value) noexcept
        : src_(new T[sz * 2]), _M_begin_(src_), _M_end_(src_ + sz),
          _M_last_(src_ + sz * 2) {

        auto new_begin = src_;
        for (int i = 0; i < sz; i++) {
            new_begin[i] = value;
        }
    }
    vector(CompactSTL::vector<T> &other) noexcept {
        src_ = new T[other.capacity()];
        std::uninitialized_copy(other.begin(), other.end(), this->begin());
        _M_begin_ = src_;
        _M_end_   = src_ + this->size();
        _M_last_  = src_ + this->capacity();
    };
    vector(CompactSTL::vector<T> &&other) noexcept {
        src_            = other.data();
        _M_begin_       = src_;
        _M_end_         = other._M_end_;
        _M_last_        = other._M_last_;
        other.src_      = nullptr;
        other._M_begin_ = nullptr;
        other._M_end_   = nullptr;
        other._M_last_  = nullptr;
    };

    Reference operator=(CompactSTL::vector<T> &other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] src_;
        src_ = new T[other.capacity()];
        std::uninitialized_copy(other.begin(), other.end(), this->begin());
        _M_begin_ = src_;
        _M_end_   = src_ + other.size();
        _M_last_  = src_ + other.capacity();
        return *this;
    }

    Reference operator=(CompactSTL::vector<T> &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        delete[] src_;
        src_            = other.data();
        _M_begin_       = src_;
        _M_end_         = other._M_end_;
        _M_last_        = other._M_last_;
        other.src_      = nullptr;
        other._M_begin_ = nullptr;
        other._M_end_   = nullptr;
        other._M_last_  = nullptr;
    }
    template <typename U> vector(U &begin_, U &end_) {
        auto sz   = std::distance(begin_, end_);
        src_      = new T[sz * 2];
        _M_begin_ = src_;
        _M_end_   = src_ + sz;

        _M_last_ = src_ + sz * 2;
        std::uninitialized_copy(begin_, end_, this->begin());
    }

    ~vector() { delete[] src_; }

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
    void clear() { _M_end_ = _M_begin_; }

    template <typename... Args> void emplace(Iterator__ it_, Args &&...arg) {
        Pointer *loc = std::to_address(it_);
        if (loc >= _M_end_) {
            ::new (static_cast<void *>(loc)) T(std::forward(arg)...);
            return;
        }
        auto next_pos = it_ + 1;
        std::copy(next_pos, end(), next_pos + 1);
    }

    template <typename... U> void emplace_back(U &&...value) {

        if (_M_end_ < _M_last_) {
            new (static_cast<void *>(_M_end_)) T(std::forward<U>(value)...);
            _M_end_ += 1; // sizeof...(U);
        } else {
            throw std::runtime_error("not yet implemented\n");
        }
    }

    void push_back(T &&value) { emplace_back(std::forward<T>(value)); }
    void pop_back() {
        if (_M_end_ > _M_begin_) {
            _M_end_--;
        }
    }

    void empty() { return _M_begin_ == _M_end_; }

    T *data() { return _M_begin_; }

    Reference operator[](size_t index) { return _M_begin_[index]; }
    Reference operator[](size_t index) const { return _M_begin_[index]; }

    Reference at(size_t idx) {
        if (idx > this->size()) {
            throw std::runtime_error("index out of bound");
        }
        return _M_begin_ + idx;
    }

    Reference at(size_t idx) const {
        if (idx > this->size()) {
            throw std::runtime_error("index out of bound");
        }
        return _M_begin_ + idx;
    }

    Reference front() { return *_M_begin_; }
    Reference front() const { return *_M_begin_; }

    Reference back() { return *_M_end_ - 1; }
    Reference back() const { return *_M_end_ - 1; }
};

} // namespace CompactSTL

struct MyType {
    int value;
    std::string name;
    double currency;
};

template <typename T> void func(T &vec) {
    vec.emplace_back(1, "noname", 100.0);
    // println("{}", vec.size());
    println("name : {} val : {} currency : {}", vec.begin()->name,
            vec.begin()->value, vec.begin()->currency);
}
int main() {

    using namespace CompactSTL;
#if 0
    std::vector<int> stdvec({1, 2, 3, 4, 5});
    CompactSTL::vector<int> vec(stdvec.begin(), stdvec.end());
    std::vector<MyType> stdvec2;

    CompactSTL::vector<int> vec2(std::move(vec));
    for (auto c : vec2) {
        println("{}", c);
    }
#endif
    std::vector<int> stdvt;
    println(
        "{}",
        std::is_trivially_constructible<CompactSTL::VectorDetail<int>>::value);
    return 0;
}