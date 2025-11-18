#ifndef PROMISE__HEADER
#define PROMISE__HEADER
#include "function.hpp"
#include "future.hpp"
#include "share_state.hpp"
#include <exception>
#include <mutex>

namespace CompactSTL {

template <typename T> class Promise {
private:
    ShareState<T> *share_state_;

public:
    Future<T> get_future() { return Future<T>(share_state_); }

    // assuming new always successes
    Promise() : share_state_(new ShareState<T>{}) {}
    Promise(const Promise &)            = delete;
    Promise &operator=(const Promise &) = delete;

    void set(T &&v) {
        std::scoped_lock<std::mutex> lock_(share_state_->mtx_);
        share_state_->value_ = std::forward<T>(v);
        share_state_->is_ready = true;
        share_state_->cv.notify_all();
    }

    void set_exception(std::exception_ptr ptr_) {
        share_state_->set_exception(ptr_);
    }
};
} // namespace CompactSTL

#endif