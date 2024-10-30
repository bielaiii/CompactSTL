#include "header.h"
#include <algorithm>
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
    UniquePtr(UniquePtr &other)            = delete;
    UniquePtr &operator=(UniquePtr &other) = delete;

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

    UniquePtr(T *ptr) noexcept : ptr(ptr) {};

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
        T *temp = ptr;
        ptr     = nullptr;
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

    auto sp1 = UniquePtr<Foo, DeleterMyPtr>(new Bar(-11, 3));

    auto stdSp = std::unique_ptr<Foo, DeleterMyPtr>(new Bar(1, 3));
    return 0;
}
