#ifndef SHARED_PTR_HEADER
#define SHARED_PTR_HEADER

#include <algorithm> // std::copy
#include <atomic>
#include <barrier>
#include <cstddef> // std::size_t
#include <cstdint>
#include <cstdio>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <print>
#include <random>
#include <type_traits>
#include <utility>
using std::cout;
using std::endl;
using std::format;
#include "someFunctionality.hpp"
namespace CompactSTL {

struct ControlBlock {
private:
    std::atomic<int> remain;

public:
public:
    ControlBlock() = delete;
    ControlBlock(int count_) noexcept : remain(std::atomic<int>(count_)) {};

    int load() { return remain.load(std::memory_order::acquire); }

    int decrease() {
        int n = remain.fetch_sub(1, std::memory_order::release);
        return n;
    }
    int increase() {
        int n = remain.fetch_add(1, std::memory_order::release);
        return n;
    }

    void reset_count() { remain.store(0, std::memory_order::release); }
};

template <typename T, typename = void>
struct is_callable : std::false_type {};

template <typename T>
struct is_callable<
    T, std::void_t<decltype(std::declval<T>()(std::declval<T *>()))>>
    : std::true_type {};

template <typename T, typename del_func = std::default_delete<T>>
class SharePointerImpl {
private:
    T *ptr;
    del_func deleter;

public:
    SharePointerImpl() noexcept : ptr(nullptr), deleter() {};
    SharePointerImpl(T *ptr_, del_func del_ = std::default_delete<T>()) noexcept
        : ptr(ptr_), deleter(std::move(del_)) {}
    ~SharePointerImpl() { deleter(ptr); }
    T *operator->() { return ptr; }
};

template <typename T>
class SharedPointer {
private:
    ControlBlock *block_            = nullptr;
    SharePointerImpl<T> *base_share = nullptr;

protected:
public:
    friend void swap(SharedPointer<T> &lhs, SharedPointer<T> &rhs) noexcept {
        std::swap(lhs.block_, rhs.block_);
        std::swap(lhs.base_share, rhs.base_share);
    }
    SharedPointer(SharedPointer<T> &&another) noexcept {
        swap(*this, another);
        auto nu_ = SharedPointer(nullptr);
        swap(another, nu_);
    }

    template <typename U, typename CurDel>
    SharedPointer(U *ptr_, CurDel delfunc_) noexcept
        : base_share(ptr_, std::move(delfunc_)) {
        if (block_ == nullptr) {
            block_ = new ControlBlock(1);
        } else {
            block_->increase();
        }
    };

    SharedPointer() = default;
    SharedPointer(std::nullptr_t) noexcept
        : block_(nullptr), base_share(nullptr) {};
    template <typename U>
    SharedPointer(U *ptr_) noexcept
        : base_share(new SharePointerImpl<U>(ptr_)),
          block_(new ControlBlock(1)){};

    ~SharedPointer() noexcept {
        if (block_ == nullptr && base_share == nullptr) {
            return;
        }
        int val = block_->decrease();
        if (val == 1) {
            delete block_;
            base_share->~SharePointerImpl();
        } else {
            return;
        }
    }

    SharedPointer(const SharedPointer<T> &myptr) noexcept {
        this->block_     = myptr.block_;
        this->base_share = myptr.base_share;
        this->block_->increase();
    }

    SharedPointer<T> &operator=(SharedPointer<T> &myptr) noexcept {
        if (this == &myptr) {
            return *this;
        }

        block_ = myptr.block_;
        if (block_) {
            block_->increase();
        }
        return *this;
    }

    bool unique() { return block_->load() == 1; }

    bool owner_before(const SharedPointer<T> &other) {
        return block_ < other.block_;
    }

    template <typename Y>
    void reset(Y *anotherPtr = nullptr) {
        auto temp = SharedPointer<Y>(anotherPtr);
        swap(temp, *this);
    }

    template <typename Del, typename = std::enable_if_t<
                                std::is_move_constructible_v<Del>, void>>
    void reset(T *anotherPtr, Del dx) {
        auto temp = SharedPointer(anotherPtr, dx);
        swap(*this, temp);
    }

    void SubtractOneCount() {
        int val = block_->decrease();
        if (val == 1) {
            delete this;
        }
    }

    size_t use_count() { return block_->load(); }

    T &operator*() { return *(base_share->operator->()); }

    T *Get() { return base_share->operator->(); }

    T *operator->() { return Get(); }

};

template <typename T, typename... Args>
SharedPointer<T> makeSharePointer(Args &&...args) {
    return SharedPointer<T>(new T(std::forward<Args>(args)...));
}
} // namespace CompactSTL

#endif