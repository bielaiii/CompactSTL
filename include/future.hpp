#ifndef FUTURE__HEADER
#define FUTURE__HEADER

#include "function.hpp"
#include "share_state.hpp"
#include <condition_variable>
#include <exception>
#include <mutex>
#include <random>
#include"SharedPtr.hpp"
namespace CompactSTL {

template <typename T> class Future {

    bool already_take_result{false};
    ShareState<T> *_M_state_;

public:
    friend void swap(Future &lhs, Future &rhs) noexcept {
        std::swap(lhs._M_state_, rhs._M_state_);
        std::swap(lhs.already_take_result, rhs.already_take_result);
    }
    Future() noexcept : already_take_result(false), _M_state_(nullptr) {};
    Future(const Future &value)       = delete;
    Future &operator=(const T &value) = delete;

    Future(Future &&ft) noexcept {
        Future tmp(std::move(ft));
        swap(tmp, *this);
    }

    Future(ShareState<T> *st_) noexcept : _M_state_(st_) {}

    void wait() {
        std::unique_lock<std::mutex> lock_(this->_M_state_->mtx_);
        _M_state_->cv.wait(lock_,
                           [this]() { return this->_M_state_->is_ready; });
    }

    bool valid() { return !already_take_result; }
    T get() {
        wait();
       
        if (_M_state_->has_exception()) {
            rethrow_exception(_M_state_->get_exception());
        }

        already_take_result = true;
        return _M_state_->get();
    };
    ~Future() {  }
};
} // namespace CompactSTL

#endif