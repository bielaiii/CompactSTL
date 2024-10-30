#include "header.h"
#include <cstdlib>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <ostream>
#include <type_traits>
#include <utility>
namespace CompactSTL {

template <typename T> T &&Move(T &&t) { return static_cast<T &&>(t); }

template <typename T, typename Deleter = std::default_delete<T>> class UniquePtr {
private:
    T *ptr;
    Deleter del;

public:
    template <typename... Args> UniquePtr() = delete;

    template <typename... Args, typename = void>
    UniquePtr(Args &&...arg) noexcept : ptr(new T(std::forward<Args>(arg)...)), del() {}

    UniquePtr(UniquePtr &other)            = delete;
    UniquePtr &operator=(UniquePtr &other) = delete;

    template <typename U, typename D = Deleter, typename = void>
    UniquePtr<U, std::void_t<typename std::is_pointer<T *>::value>>(T *ptr, D deleter) : ptr(ptr), del(deleter){};

    UniquePtr(UniquePtr &&other) noexcept : ptr(other.ptr), del(other.del) {};
    UniquePtr &operator=(UniquePtr &&other) noexcept {
        if (this != other) {
            del(ptr);
            ptr = other.ptr;
            del = other.del;

            other.ptr = nullptr;
        }
        return *this;
    };

    explicit UniquePtr(T *ptr) noexcept : ptr(ptr) {};

    ~UniquePtr() noexcept {
        del(ptr);
        std::cout << "unique point get released\n";
    }

    T &operator->() { return ptr; }

    void reset(T *t = nullptr) {
        T *old_ptr = this->ptr;
        this->ptr  = t;
        if (old_ptr) {
            delete (old_ptr);
        }
    }

    T *get() { return ptr; }

    friend std::ostream &operator<<(std::ostream &os, UniquePtr<T> &t) {
        std::cout << t.get();
        return os;
    }

    void swap(UniquePtr<T> &t) { UniquePtr<T> temp(t); }
    T *release() {
        T *temp   = this->ptr;
        this->ptr = nullptr;
        return temp;
    }
};
}; // namespace CompactSTL

void FreeMyPtr(Foo *ptr) { delete ptr; }

int main() {
    using namespace CompactSTL;
    UniquePtr<Foo> sp(1, 2.2);
    auto it = sp.release();
    std::cout << it << "\n";

    // auto delFoo = std::function<FreeMyPtr<Foo*>()>;
    auto delFoo = [](Foo *foo) { delete foo; };
    auto rawPtr = new Foo(1, 3);
    auto ir     = FreeMyPtr;
    auto sp1    = UniquePtr<Foo>(static_cast<Foo *>(rawPtr), FreeMyPtr);

    return 0;
}
