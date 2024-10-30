#include "header.h"
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
        if (ptr) {
            del(ptr);
        }
        std::cout << "unique point get released\n";
    }

    T *operator->() { return ptr; }

    void reset(T *t = nullptr) {
        T *old_ptr = this->ptr;
        this->ptr  = t;
        if (old_ptr) {
            delete (old_ptr);
        }
    }

    T *get() { return ptr; }

    void swap(UniquePtr<T> &other) {
        std::swap(ptr, other.ptr);
        std::swap(del, other.del);
    }
    T *release() {
        T * temp = ptr;
        ptr = nullptr;
        return temp;
    }
};
}; // namespace CompactSTL

void FreeMyPtr(Foo *ptr) { delete ptr; }

struct DeleterMyPtr {
    void operator()(Foo *f) { delete (f); }
};

int main() {
    using namespace CompactSTL;
    UniquePtr<Foo> sp(1, 2.2);
    auto it = sp.release();
    std::cout << it << "\n";
    delete it;

    auto rawPtr = new Foo(1, 3);
    auto sp1    = UniquePtr<Foo, DeleterMyPtr>(rawPtr);
    cout << sp1.get() << "\n";
    return 0;
}
