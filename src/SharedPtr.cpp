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

    int operator++() {
        int n = remain.fetch_sub(1);
        return n;
    }
    int operator--() {
        int n = remain.fetch_sub(1);
        return n;
    }
    int decrease() {
        int n = remain.fetch_sub(1);
        return n;
    }
    int increase() {
        int n = remain.fetch_add(1);
        return n;
    }
};

template <typename T, typename = void> struct is_callable : std::false_type {};

template <typename T>
struct is_callable<
    T, std::void_t<decltype(std::declval<T>()(std::declval<T *>()))>>
    : std::true_type {};

template <typename T> class SharePointerImpl {
public:
    template <typename D>
    SharePointerImpl(T *ptr_, D del_ = std::default_delete<T>()) noexcept
        : ptr(ptr_) {}

private:
    T *ptr;
};

template <typename T> class SharedPointer {
private:
    ControlBlock *block_ = nullptr;
    T *ptr;
    std::function<void(T *)> delfunc;

public:
    template <typename U, typename CurDel>
    SharedPointer(U *ptr_, CurDel delfunc_) noexcept
        : ptr(ptr_), delfunc(delfunc_) {
        if (block_ == nullptr) {
            block_ = new ControlBlock(1);
        } else {
            block_->increase();
        }
    };

    template <typename U>
    SharedPointer(U *ptr_) noexcept
        : delfunc(std::default_delete<T>()), ptr(ptr_) {
        if (block_ == nullptr) {
            block_ = new ControlBlock(1);
        } else {
            block_->increase();
        }
    };

    ~SharedPointer() noexcept {
        int val = block_->decrease();
        if (val == 1) {
            delete block_;
            delfunc(ptr);
        } else {
            return;
        }
    }

    SharedPointer(const SharedPointer<T> &myptr) noexcept {
        this->block_ = myptr.block_;
        block_->increase();
    }

    SharedPointer(SharedPointer<T> &&myptr) noexcept {
        this->block_ = myptr.block_;
        myptr.block_ = nullptr;
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

    void Release(const SharedPointer<T> &ptr = nullptr) {
        ptr.block_->remain = 0;
        delete ptr;
        delete block_;
    }

    void reset(T *anotherPtr = nullptr) {
        SubtractOneCount();
        if (anotherPtr) {
            block_ = new ControlBlock(1, anotherPtr);
        }
    }

    void SubtractOneCount() {
        int val = block_->decrease();
        if (val == 1) {
            delete this;
        }
    }

    T *operator*() { return *ptr; }

    T *Get() { return this->t; }

    T *operator->() { return ptr; }
};

template <typename T, typename... Args>
SharedPointer<T> makeSharePointer(Args &&...args) {
    return SharedPointer<T>(new T(std::forward<Args>(args)...));
}
} // namespace CompactSTL

struct Foo {
    int a;
    double d;
    Foo() : a(-2), d(0) { std::cout << "Foo default construct\n"; };
    Foo(int a_, double d_) : a(a_), d(d_) { std::cout << "Foo construct\n"; };

    template <typename T, std::enable_if<std::is_same_v<std::decay_t<T>, Foo>>>
    friend std::ostream &operator<<(std::ostream &os, T f) {
        os << format("a : {}, d : {}!!!!\n", f->a, f->d);
        return os;
    }

    virtual ~Foo() { std::cout << "Foo destruct\n"; };
};

struct Bar : Foo {
    float x;
    float y;
    Bar() : x(-2), y(0) { cout << "Bar construct\n"; };
    Bar(float x_, float y_) : x(x_), y(y_) { cout << "Bar construct\n"; };
    virtual ~Bar() { cout << "Bar destruct\n"; };
};

template <typename T> void PrintSp(CompactSTL::SharedPointer<T> sp) {
    cout << &sp << "\n";
}

void DeleteFunc(Foo *t) {
    cout << "callde from T*\n";
    delete t;
}

int main() {
    using namespace CompactSTL;

    auto sp1     = std::shared_ptr<Foo>(new Foo(12, 3.3), DeleteFunc);
    auto tempdel = [](Foo *foo) {
        cout << "delete from lambda func\n";
        delete foo;
    };

    {
        auto sp4 = SharedPointer<Foo>(new Foo(12, 3.3), tempdel);
        auto sp5 = sp4;
        cout << "will not yet destruct\n";
    }

    auto sp2 = CompactSTL::SharedPointer<Foo>(new Foo(1, 3.3), DeleteFunc);
    return 0;
}
