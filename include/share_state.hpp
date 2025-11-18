#ifndef SHARE_STATE__HEADER
#define SHARE_STATE__HEADER
#include <condition_variable>
#include <mutex>
namespace CompactSTL {

template <typename T> class ShareState {
    public:

    T value_;
    bool is_ready{false};
    std::condition_variable cv;
    std::mutex mtx_;
    std::exception_ptr exception_ptr{nullptr};

    ShareState() = default;

    bool has_exception() noexcept { return exception_ptr != nullptr; }

    std::exception_ptr get_exception() noexcept { return exception_ptr; }

    T get() noexcept { return value_; }

    T set(T v) noexcept {
        std::scoped_lock<std::mutex> lock_(mtx_);
        is_ready = true;
        value_   = v;
    }

    void set_exception(std::exception_ptr ptr) noexcept { exception_ptr = ptr; }
};
} // namespace CompactSTL

#endif