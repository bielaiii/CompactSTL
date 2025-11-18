#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>

using std::cout;
using std::endl;

namespace CompactSTL {

template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
private:
    T *ptr = nullptr;

    Deleter getDeleter() { return Deleter{}; }

public:
    friend void swap(UniquePtr<T> &lhs, UniquePtr<T> &rhs) {
        std::swap(lhs.ptr, rhs.ptr);
    }
    UniquePtr(UniquePtr &other)            = delete;
    UniquePtr &operator=(UniquePtr &other) = delete;

    UniquePtr(UniquePtr &&other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    };
    UniquePtr &operator=(UniquePtr &&other) noexcept {
        if (this != other) {
            this->ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    };

    UniquePtr(T *ptr) noexcept : ptr(ptr) {};
    
    ~UniquePtr() noexcept {
        if (ptr) {
            getDeleter()(ptr);
        }
    }

    T *operator->() { return ptr; }

    T &operator*() { return *ptr; };

    void reset(T *t = nullptr) {
        auto old_ptr = ptr;
        ptr          = t;
        if (old_ptr) {
            getDeleter()(old_ptr);
        }
    }

    T *get() { return ptr; }

    T *release() {
        T *temp = ptr;
        ptr     = nullptr;
        return temp;
    }

    void swap(UniquePtr<T> &other) { std::swap(other.ptr, this->ptr); }
};

template <typename T, typename... Args>
UniquePtr<T> MakeUnique(Args &&...args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}
}; // namespace CompactSTL
