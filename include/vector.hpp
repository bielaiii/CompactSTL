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
    T *_M_begin_{};
    T *_M_end_{};
    T *_M_last_{};

    using Reference         = VectorDetail<T> &;
    using Pointer           = VectorDetail<T> *;
    VectorDetail() noexcept = default;
    VectorDetail(T *begin_, T *end_, T *last_) noexcept
        : _M_begin_(begin_), _M_end_(end_), _M_last_(last_) {};
    VectorDetail(T *begin, size_t sz) noexcept
        : _M_begin_(begin), _M_end_(begin), _M_last_(begin + sz) {};
    VectorDetail(size_t capacity_) noexcept {
        _M_begin_ = static_cast<T *>(::operator new(sizeof(T[capacity_])));
        _M_end_   = _M_begin_;
        _M_last_  = _M_begin_ + capacity_;
    }

    VectorDetail(const VectorDetail<T> &other) noexcept {
        auto size_     = other._M_end_ - other._M_begin_ ;
        auto capacity_ = other._M_last_ - other._M_begin_ ;
        _M_begin_ = static_cast<T*>(::operator new(sizeof(T) * capacity_));
        _M_end_   = _M_begin_ + size_;
        _M_last_  = _M_begin_ + capacity_;
    }
    VectorDetail(VectorDetail<T> &&other) noexcept {
        swap(other);
        VectorDetail<T>{}.swap(other);
    }
    bool operator!=(const VectorDetail<T> &other) {
        return _M_begin_ != other._M_begin_;
    }
    friend void swap(VectorDetail<T> &new_dst,
                     VectorDetail<T> &old_dst) noexcept {
        using std::swap;
        swap(new_dst._M_begin_, old_dst._M_begin_);
        swap(new_dst._M_end_, old_dst._M_end_);
        swap(new_dst._M_last_, old_dst._M_last_);
    }
    VectorDetail<T> &operator=(VectorDetail<T> other) noexcept {
        if (*this != other) {
            VectorDetail<T> temp(other);
            swap(*this, temp);
        }
        return *this;
    }
    VectorDetail<T> &operator=(VectorDetail<T> &&other) noexcept {
        if (this != &other) {
            VectorDetail<T> temp(std::move(other));
            swap(*this, temp);
        }
        return *this;
    }
    void swap(VectorDetail<T> &other) noexcept {
        using std::swap;
        swap(*this, other);
    }
    ~VectorDetail() { ::operator delete(_M_begin_); }
};

template <typename T> class vector {
private:
    VectorDetail<T> _M_detail_;

    void destruct_elements() {
        for (auto &dt : *this) {
            dt.~T();
        }
    }

public:
    using Pointer         = T *;
    using Reference       = T &;
    using Iterator__      = Iterator<T>;
    using RIterator__     = Reverse_iterator<T>;
    using difference_type = std::ptrdiff_t;

    vector(size_t capacity_ = 10) noexcept : _M_detail_(capacity_ * 2) {};
    vector(size_t capacity_, const T &value) noexcept : _M_detail_(capacity_) {
        std::uninitialized_fill_n(begin(), capacity_, value);
        _M_detail_._M_end_ = _M_detail_._M_begin_ + capacity_;
    }
    vector(const CompactSTL::vector<T> &other) noexcept
        : _M_detail_(other._M_detail_) {
        std::uninitialized_copy(other.begin(), other.end(), begin());
        auto sz            = std::distance(other.begin(),
                                          other.end());
        _M_detail_._M_end_ = _M_detail_._M_begin_ + sz;
    };
    vector(CompactSTL::vector<T> &&other) noexcept
        : _M_detail_(std::move(other._M_detail_)) {};

    vector<T> &operator=(CompactSTL::vector<T> other) noexcept {
        if (this == &other) {
            return *this;
        }
        _M_detail_.swap(other._M_detail_);
        return *this;
    }

    template <typename U>
    vector(U &begin_, U &end_) noexcept
        : _M_detail_(std::distance(begin_, end_)) {
        auto sz = std::distance(begin_, end_);
        ;

        std::uninitialized_copy(begin_, end_, this->begin());
        _M_detail_._M_end_ = _M_detail_._M_begin_ + sz;
    }

    vector(std::initializer_list<T> &init_list) noexcept
        : _M_detail_(init_list.size()) {
        std::uninitialized_copy(init_list.begin(), init_list.end(),
                                this->begin());
        _M_detail_._M_end_ = _M_detail_._M_begin_ + init_list.size();
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
        return Iterator__(_M_detail_._M_begin_);
    }
    [[nodiscard]] Iterator__ begin() const {
        return Iterator__{_M_detail_._M_begin_};
    }
    [[nodiscard]] Iterator__ cbegin() const {
        return Iterator__{_M_detail_._M_begin_};
    }
    [[nodiscard]] Iterator__ end() { return Iterator__{_M_detail_._M_end_}; }
    [[nodiscard]] Iterator__ end() const {
        return Iterator__{_M_detail_._M_end_};
    }
    [[nodiscard]] Iterator__ cend() const {
        return Iterator__{_M_detail_._M_end_};
    }

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
    void clear() noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            for (auto &dt : *this) {
                dt.~T();
            }
        }
        _M_detail_._M_end_ = _M_detail_._M_begin_;
    }

    void resize(size_t new_size) {
        if (capacity() >= new_size) {
            return;
        }

        VectorDetail<T> new_detail_(new_size);
        std::uninitialized_move(_M_detail_._M_begin_, _M_detail_._M_end_,
                                new_detail_._M_begin_);
        swap(_M_detail_, new_detail_);
        VectorDetail<T>{}.swap(new_detail_);
    }

    template <typename... Args> void emplace(Iterator__ it_, Args &&...arg) {
        Pointer loc = std::to_address(it_);
        if (size() + 1 <= capacity()) {
            auto next_pos = std::next(it_, 1);
            
            std::uninitialized_move(it_, end(), next_pos);
            ::new (loc) T(std::forward<Args>(arg)...);
            _M_detail_._M_end_++;
            return;
        }

        vector<T> new_vec(capacity() * 2);
        auto next_pos_ = std::next(it_);
        std::uninitialized_move(begin(), it_, new_vec.begin());
        auto new_element = new_vec.begin();
        std::advance(new_element, std::distance(begin(), it_));

        ::new (std::to_address(new_element)) T(std::forward<Args>(arg)...);
        new_element  = std::next(new_element);
        std::uninitialized_move(it_, end(), new_element);
        new_vec._M_detail_._M_end_++;
        _M_detail_.swap(new_vec._M_detail_);
        VectorDetail<T>{}.swap(new_vec._M_detail_);
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

    void push_back(T &&value) { emplace_back(std::move(value)); }

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

    void swap(vector<T> &other) noexcept {
        using std::swap;
        swap(_M_detail_, other._M_detail_);
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

    ~vector() noexcept {
        if constexpr (!std::is_trivially_destructible_v<T>) {
            destruct_elements();
        }
      
    }
};

} // namespace CompactSTL