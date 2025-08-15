#include "iterator.hpp"
#include <initializer_list>
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
    bool operator!=(const VectorDetail<T> &other) {
        return _M_begin_ != other._M_begin_;
    }
    friend void swap(VectorDetail<T> &new_dst,
                     VectorDetail<T> &old_dst) noexcept {
        using namespace std;
        swap(new_dst._M_begin_, old_dst._M_begin_);
        swap(new_dst._M_end_, old_dst._M_end_);
        swap(new_dst._M_last_, old_dst._M_last_);
    }
    VectorDetail<T> operator=(VectorDetail<T> other) noexcept {
        if (*this != other) {
            VectorDetail<T> temp(other);
            swap(*this, temp);
        }
        return *this;
    }
    VectorDetail<T> operator=(VectorDetail<T> &&other) noexcept {
        if (this != &other) {
            VectorDetail<T> temp(std::move(other));
            swap(*this, temp);
        }
        return *this;
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
    void partital_move(Iterator<T> beginer_, Iterator<T> ender_,
                       Iterator<T> inser_pos, Iterator<T> new_pos) {
        auto dis = std::distance(beginer_, inser_pos);
        std::uninitialized_move(beginer_, beginer_ + dis - 1, new_pos);
        new_pos += dis - 1;
        beginer_ += dis - 1;
        std::uninitialized_move(beginer_, ender_, new_pos);
    }

public:
    using Pointer     = T *;
    using Reference   = T &;
    using Iterator__  = Iterator<T>;
    using RIterator__ = Reverse_iterator<T>;

    vector(size_t capacity_ = 10) noexcept : _M_detail_(capacity_ * 2) {};
    vector(size_t capacity_, const T &value) noexcept : _M_detail_(capacity_) {
        std::uninitialized_fill_n(begin(), capacity_, value);
        _M_detail_._M_end_ = _M_detail_._M_begin_ + capacity_;
    }
    vector(CompactSTL::vector<T> &other) noexcept
        : _M_detail_(other._M_detail_) {
        std::uninitialized_copy(other.begin(), other.end(), begin());
        auto sz            = std::distance(std::to_address(other.begin()),
                                           std::to_address(other.end()));
        _M_detail_._M_end_ = _M_detail_._M_begin_ + sz;
    };
    vector(CompactSTL::vector<T> &&other) noexcept
        : _M_detail_(std::move(other._M_detail_)) {};

    vector<T> &operator=(CompactSTL::vector<T> other) noexcept {
        if (this == &other) {
            return *this;
        }
        swap(_M_detail_, other._M_detail_);
        return *this;
    }

    template <typename U> vector(U &begin_, U &end_) {
        auto sz    = std::distance(begin_, end_);
        _M_detail_ = VectorDetail<T>(sz);

        std::uninitialized_copy(begin_, end_, this->begin());
    }

    vector(std::initializer_list<T> &init_list) noexcept {
        auto sz = init_list.size();
    };

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
    void clear() {
        if constexpr (std::is_standard_layout_v<T> && std::is_trivial_v<T>) {
            _M_detail_._M_end_ = _M_detail_._M_begin_;
        } else {
            for (T *start_ = _M_detail_._M_begin_; start_ < _M_detail_._M_end_;
                 start_++) {
                start_->~T();
            }
        }
    }

    template <typename... Args> void emplace(Iterator__ it_, Args &&...arg) {
        Pointer *loc = std::to_address(it_);
        if (size() + 1 > capacity()) {

            return;
        }

        if (loc == _M_detail_._M_end_) {
            ::new (static_cast<void *>(loc)) T(std::forward<Args>(arg)...);
            _M_detail_._M_end_ += 1;
            return;
        } else if (loc < _M_detail_._M_end_) {

        } else {
            
        }

        auto next_pos = it_ + 1;
        std::copy(next_pos, end(), next_pos + 1);
    }

    template <typename... U> void emplace_back(U &&...value) {

        if (_M_detail_._M_end_ < _M_detail_._M_last_) {
            new (static_cast<void *>(_M_detail_._M_end_))
                T(std::forward<U>(value)...);
            _M_detail_._M_end_ += 1;
        } else {
            throw std::runtime_error("not yet implemented\n");
        }
    }

    void push_back(T &&value) { emplace_back(std::move<T>(value)); }

    void push_back(const T &value) { emplace_back(value); }

    void pop_back() {
        if (empty()) {
            return;
        }
        if constexpr (std::is_standard_layout_v<T> &&
                      std::is_trivially_destructible_v<T>) {

            if (_M_detail_._M_end_ > _M_detail_._M_begin_) {
                _M_detail_._M_end_--;
            }
        } else {
            T *last_v = _M_detail_._M_end_ - 1;
            last_v->~T();
            _M_detail_._M_end_--;
        }
    }

    bool empty() { return _M_detail_._M_begin_ == _M_detail_._M_end_; }

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
        return *(_M_detail_._M_begin_ + idx);
    }

    Reference front() { return *_M_detail_._M_begin_; }
    Reference front() const { return *_M_detail_._M_begin_; }

    Reference back() { return *(_M_detail_._M_end_ - 1); }
    Reference back() const { return *(_M_detail_._M_end_ - 1); }
};

} // namespace CompactSTL