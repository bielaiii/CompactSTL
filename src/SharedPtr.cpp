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

    int load() { return remain.load(); }

    int decrease() {
        int n = remain.fetch_sub(1);
        return n;
    }
    int increase() {
        int n = remain.fetch_add(1);
        return n;
    }

    void reset_count() { remain.store(-0); }
};

template <typename T, typename = void> struct is_callable : std::false_type {};

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

template <typename T, typename delfunc = std::default_delete<T>>
struct temporary_owner {
    T *ptr;
    delfunc deleter;
    bool pass_owner;
    temporary_owner() = delete;
    temporary_owner(T *p, delfunc del) noexcept
        : ptr(p), deleter(del), pass_owner(false) {};
    T *operator*() { return ptr; };
    ~temporary_owner() {

        if (pass_owner) {

        } else {

            deleter(ptr);
        }
    }
};

template <typename T> class SharedPointer {
private:
    ControlBlock *block_ = nullptr;
    SharePointerImpl<T> *base_share;

protected:
    template <
        typename U, typename DelcFunc,
        typename = std::enable_if_t<std::is_move_constructible_v<DelcFunc>>>
    void safely_build_base_(U *u, DelcFunc delc) {
        auto temp_owner       = temporary_owner<U, DelcFunc>(u, delc);
        base_share            = new SharePointerImpl(temp_owner.ptr);
        temp_owner.pass_owner = true;
        delete base_share;
    }

public:
    template <typename U, typename CurDel>
    SharedPointer(U *ptr_, CurDel delfunc_) noexcept
        : base_share(ptr_, std::move(delfunc_)) {
        if (block_ == nullptr) {
            block_ = new ControlBlock(1);
        } else {
            block_++;
        }
    };

    SharedPointer() = default;

    template <typename U> SharedPointer(U *ptr_ = nullptr) noexcept {
        safely_build_base_(ptr_, std::default_delete<U>());
    };

    ~SharedPointer() noexcept {
        int val = block_->decrease();
        std::println("remain count {}", val);
        if (val == 1) {
            delete block_;
            base_share->~SharePointerImpl();
        } else {
            // cout << "remain value " << val - 1 << "\n";
            return;
        }
    }

    SharedPointer(const SharedPointer<T> &myptr) noexcept {
        this->block_     = myptr.block_;
        this->base_share = myptr.base_share;
        this->block_->increase();
    }

    SharedPointer(SharedPointer<T> &&another) noexcept {
        this->block_     = another.block_;
        this->base_share = another.base_share;

        another.block_     = nullptr;
        another.base_share = nullptr;
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

    void reset(T *anotherPtr = nullptr) {
        SharedPointer(anotherPtr).swap(*anotherPtr);
    }

    template <typename Del, typename = std::enable_if_t<
                                std::is_move_constructible_v<Del>, void>>
    void reset(T *anotherPtr, Del dx) {
        SharedPointer(anotherPtr, dx).swap(*anotherPtr);
    }

    void SubtractOneCount() {
        int val = block_->decrease();
        if (val == 1) {
            delete this;
        }
    }

    size_t use_count() { return block_->load(); }

    T &operator*() { return *(base_share->ptr); }

    T *Get() { return base_share->ptr; }

    T *operator->() { return Get(); }

    friend void swap(SharedPointer<T> &lhs, SharedPointer<T> &rhs) {
        auto temp_base_  = lhs.base_share;
        auto temp_block_ = lhs.block_;

        lhs.block_     = std::move(rhs.block_);
        lhs.base_share = rhs.base_share;

        rhs.block_     = temp_block_;
        rhs.base_share = temp_base_;
    }

    void swap(SharedPointer<T> &other) { swap(*this, other); }
};

template <typename T, typename... Args>
SharedPointer<T> makeSharePointer(Args &&...args) {
    return SharedPointer<T>(new T(std::forward<Args>(args)...));
}
} // namespace CompactSTL

struct Foo {
    int a;
    double d;
    Foo()
        : a(-2), d(0) {
              // std::cout << "Foo default construct\n";
          };
    Foo(int a_, double d_)
        : a(a_), d(d_) {
              //
              // std::cout << "Foo construct\n";
          };

    template <typename T, std::enable_if<std::is_same_v<std::decay_t<T>, Foo>>>
    friend std::ostream &operator<<(std::ostream &os, T f) {
        os << format("a : {}, d : {}!!!!\n", f->a, f->d);
        return os;
    }

    virtual ~Foo() {

        //    std::cout << "Foo destruct\n";
    };
};

struct Bar : Foo {
    float x;
    float y;
    Bar()
        : x(-2), y(0) {

          };
    Bar(float x_, float y_)
        : x(x_), y(y_) {
              // cout << "Bar construct\n";
          };
    virtual ~Bar() {

        // cout << "Bar destruct\n";
        //
    };
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
    using std::println;
    auto fpr              = new Foo();
    SharedPointer<Foo> f1 = SharedPointer<Foo>(fpr);

    {

        SharedPointer<Foo> sp3(f1);
        SharedPointer<Foo> sp4(f1);

        println("outer {}", sp3.use_count());
    }
    println("out {}", f1.use_count());

    // println("")
    // cout << sp2.
#if ENABLE_STD_CHECK
    using namespace std;
    auto stdsp1            = make_shared<Foo>(1, 2);
    shared_ptr<Foo> stdsp2 = stdsp1;
    shared_ptr<Foo> stdsp3 = stdsp1;
    // auto stdsp3 = make_shared<Foo>(1, 2);
    cout << stdsp1.use_count() << endl;
    cout << stdsp2.use_count() << endl;
    cout << stdsp3.use_count() << endl;
    stdsp1.reset();

    cout << stdsp1.use_count() << endl;
    cout << stdsp2.use_count() << endl;
    cout << stdsp3.use_count() << endl;
#endif

    return 0;
}
