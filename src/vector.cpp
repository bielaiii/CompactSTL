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

template <typename T> class VectorDetail {
public:
    T *_M_begin_;
    T *_M_end_;
    T *_M_last_;

    using Reference = VectorDetail<T> &;
    using Pointer   = VectorDetail<T> *;
    VectorDetail()  = default;
    VectorDetail(T *begin_, T *end_, T *last_) noexcept
        : _M_begin_(begin_), _M_end_(end_), _M_last_(last_) {};
    VectorDetail(T *begin, size_t sz) noexcept
        : _M_begin_(begin), _M_end_(begin), _M_last_(begin + sz) {};
    VectorDetail(size_t capacity_) noexcept {
        _M_begin_ = static_cast<T *>(::operator new(sizeof(T[capacity_])));
        _M_end_   = _M_begin_;
        _M_last_  = _M_begin_ + capacity_;
    }

    VectorDetail(VectorDetail<T> &other) noexcept {
        auto size_     = std::distance(other._M_begin_, other._M_end_);
        auto capacity_ = std::distance(other._M_begin_, other._M_last_);
        _M_begin_ = static_cast<T *>(::operator new(sizeof(T) * capacity_));
        _M_end_   = _M_begin_ + size_;
        _M_last_  = _M_begin_ + capacity_;
    }
    VectorDetail(VectorDetail<T> &&other) noexcept {
        auto size       = _M_last_ - _M_begin_;
        _M_begin_       = other._M_begin_;
        _M_last_        = other._M_last_;
        _M_end_         = other._M_end_;
        other._M_begin_ = nullptr;
        other._M_end_   = nullptr;
        other._M_last_  = nullptr;
    }
    Reference operator=(VectorDetail<T> other) noexcept {
        if (*this != other) {
            VectorDetail<T> temp(other);
            swap(*this, temp);
        }
        return *this;
    }
    Reference operator=(VectorDetail<T> &&other) noexcept {
        if (this != &other) {
            VectorDetail<T> temp(std::move(other));
            swap(*this, temp);
        }
        return *this;
    }

    friend void swap(T &&lhs, T &&rhs) noexcept {
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
    VectorDetail<T> _M_detail_;

    void resize() {
        auto old_szie = size();
        auto new_size = old_szie * 2;

        T *new_dst    = new T[new_size];
        auto new_iter = Iterator<T>(new_dst);
        std::uninitialized_copy(begin(), end(), new_iter);
    }

public:
    using Pointer     = T *;
    using Reference   = T &;
    using Iterator__  = Iterator<T>;
    using RIterator__ = Reverse_iterator<T>;

    vector(size_t sz = 10) noexcept : _M_detail_(sz * 2) {};
    vector(size_t sz, const T &value) noexcept : vector(sz) {
        std::uninitialized_fill_n(begin(), sz, value);
    }
    vector(CompactSTL::vector<T> &other) noexcept
        : _M_detail_(other._M_detail_) {
        std::uninitialized_copy(other.begin(), other.end(), begin());
    };
    vector(CompactSTL::vector<T> &&other) noexcept
        : _M_detail_(std::move(other._M_detail_)) {};

    Reference operator=(CompactSTL::vector<T> &other) noexcept {
        if (this == &other) {
            return *this;
        }
        _M_detail_ = other._M_detail_;
        return *this;
    }

    Reference operator=(CompactSTL::vector<T> &&other) noexcept {
        if (this == &other) {
            return *this;
        }
        _M_detail_ = other._M_detail_;
        return *this;
    }
    template <typename U> vector(U &begin_, U &end_) {
        auto sz    = std::distance(begin_, end_);
        _M_detail_ = VectorDetail<T>(sz);

        std::uninitialized_copy(begin_, end_, this->begin());
    }

    //~vector() { _M_detail_.~VectorDetail(); }

    [[nodiscard]] size_t size() {
        return _M_detail_._M_end_ - _M_detail_._M_begin_;
    }

    [[nodiscard]] size_t size() const {
        return _M_detail_._M_end_ - _M_detail_._M_begin_;
    }
    [[nodiscard]] size_t capacity() {
        return _M_detail_._M_last_ - _M_detail_._M_begin_;
    }
    [[nodiscard]] size_t capacity() const {
        return _M_detail_._M_last_ - _M_detail_._M_begin_;
    }

    [[nodiscard]] Iterator__ begin() {
        return Iterator__{_M_detail_._M_begin_};
    }
    [[nodiscard]] Iterator__ cbegin() {
        return Iterator__{_M_detail_._M_begin_};
    }
    [[nodiscard]] Iterator__ end() { return Iterator__{_M_detail_._M_end_}; }
    [[nodiscard]] Iterator__ cend() { return Iterator__{_M_detail_._M_end_}; }

    [[nodiscard]] RIterator__ rbegin() {
        return RIterator__{_M_detail_._M_end_ - 1};
    }
    [[nodiscard]] RIterator__ crbegin() {
        return RIterator__{_M_detail_._M_end_ - 1};
    }
    [[nodiscard]] RIterator__ rend() {
        return RIterator__{_M_detail_._M_begin_ - 1};
    }
    [[nodiscard]] RIterator__ crend() {
        return RIterator__{_M_detail_._M_begin_ - 1};
    }

    T *operator*() { return _M_detail_._M_begin_; }

    void shrink_to_fit() {}
    void clear() { _M_detail_._M_end_ = _M_detail_._M_begin_; }

    template <typename... Args> void emplace(Iterator__ it_, Args &&...arg) {
        Pointer *loc = std::to_address(it_);
        if (loc >= _M_detail_._M_end_) {
            ::new (static_cast<void *>(loc)) T(std::forward<Args>(arg)...);
            _M_detail_._M_end_ += 1;
            return;
        }
        auto next_pos = it_ + 1;
        std::copy(next_pos, end(), next_pos + 1);
    }

    template <typename... U> void emplace_back(U &&...value) {

        if (_M_detail_._M_end_ < _M_detail_._M_last_) {
            new (static_cast<void *>(_M_detail_._M_end_))
                T(std::forward<U>(value)...);
            _M_detail_._M_end_ += 1; // sizeof...(U);
        } else {
            throw std::runtime_error("not yet implemented\n");
        }
    }

    void push_back(T &&value) { emplace_back(std::forward<T>(value)); }
    void pop_back() {
        if (_M_detail_._M_end_ > _M_detail_._M_begin_) {
            _M_detail_._M_end_--;
        }
    }

    void empty() { return _M_detail_._M_begin_ == _M_detail_._M_end_; }

    T *data() { return _M_detail_._M_begin_; }

    Reference operator[](size_t index) noexcept {
        return _M_detail_._M_begin_[index];
    }
    Reference operator[](size_t index) const noexcept {
        return _M_detail_._M_begin_[index];
    }

    Reference at(size_t idx) {
        if (idx >= this->size()) {
            throw std::runtime_error("index out of bound");
        }
        return *(_M_detail_._M_begin_ + idx);
    }

    Reference at(size_t idx) const {
        if (idx >= this->size()) {
            throw std::runtime_error("index out of bound");
        }
        return _M_detail_._M_begin_ + idx;
    }

    Reference front() { return *_M_detail_._M_begin_; }
    Reference front() const { return *_M_detail_._M_begin_; }

    Reference back() { return *(_M_detail_._M_end_ - 1); }
    Reference back() const { return *(_M_detail_._M_end_ - 1); }
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
/* int main() {

    using namespace CompactSTL;

    return 0;
} */