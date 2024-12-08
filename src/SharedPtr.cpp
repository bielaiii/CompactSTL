#include <algorithm> // std::copy
#include <atomic>
#include <barrier>
#include <cstddef> // std::size_t
#include <cstdint>
#include <cstdio>
#include <format>
#include <iostream>
#include <memory>
#include <ostream>
#include <random>
#include <type_traits>
#include <utility>
using std::cout;
using std::endl;
using std::format;

namespace CompactSTL {

template <typename T> struct ControlBlock {
    T *ptr;
    std::atomic<int> remain;
    ControlBlock() = delete;
    ControlBlock(int remain, T *ptr) : remain(remain), ptr(ptr) {};
    ~ControlBlock() { delete ptr; }
    int decrease() {
        int n = remain.fetch_sub(1);
        return n;
    }
    int increase() {
        int n = remain.fetch_add(1);
        return n;
    }
};

template <typename T, typename DelFunc = std::default_delete<T>>
class SharedPointer {
private:
    ControlBlock<T> *block_ = nullptr;

public:
    DelFunc selfDel;

    explicit SharedPointer(T * ptr, DelFunc delfunc) noexcept {
        if (block_ == nullptr) {
            block_ = new ControlBlock<T>(1, new (T));
            selfDel = delfunc;
        } else {
            block_->increase();
        }
    };
    ~SharedPointer() noexcept {
        int val = block_->decrease();
        if (val == 1) {
            delete block_;
        } else {
            return;
        }
    }

    /* template <typename Y, typename Deleter>
    SharedPointer(std::nullptr_t ptr, Deleter del) {} */


    SharedPointer(SharedPointer<T> &myptr) {
        this->block_ = myptr.block_;
        block_->increase();
    }

    SharedPointer(SharedPointer<T> &&myptr) { this->block_ = myptr.block_; }
    SharedPointer<T> &operator=(SharedPointer<T> &myptr) {
        if (this == &myptr) {
            return *this;
        }
        myptr.remain->fetch_add(1);
        delete myptr;

        block_->remain = myptr.remain;
        block_->t      = myptr.t;
        return *this;
    }

    size_t use_count() { return *block_->remain; }

    friend std::ostream &operator<<(std::ostream &os,
                                    const SharedPointer<T> &myptr) {
        os << format("src addr : 0x{:p} remian addr : 0x{:p} cnt : {}\n",
                     static_cast<void *>(&myptr.block_->remain),
                     static_cast<void *>(myptr.block_->ptr),
                     (myptr.block_->remain.load()));
        return os;
    }

    void Release(const SharedPointer<T> &ptr = nullptr) {
        ptr.block_->remain = 0;
        delete block_->ptr;
        delete block_;
    }

    void reset(T *anotherPtr = nullptr) {
        SubtractOneCount();
        if (anotherPtr) {
            // delete block_;
            block_ = new ControlBlock(1, anotherPtr);
        }
    }

    void SubtractOneCount() {
        int val = block_->decrease();
        if (val == 1) {
            delete this;
        }
    }

    T *operator*() { return *block_->t; }

    T *Get() { return this->t; }

    T *operator->() { return block_->ptr; }
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

template <typename T> void DeleteFunc(T *t) {
    cout << "callde from func\n";
    delete t;
}

int main() {
    using namespace CompactSTL;
    //auto temp = SharedPointer<Foo, DeleteFUnc<Foo>>(1, 3.3);
    // todo: bug to fix
    auto sp2 = SharedPointer<Foo>(new Foo(1, 3.3), DeleteFunc);


    auto sp1 = std::shared_ptr<Foo>(new Foo(12, 3.3));

    return 0;
}
