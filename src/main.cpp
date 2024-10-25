#include <algorithm> // std::copy
#include <atomic>
#include <barrier>
#include <cstddef> // std::size_t
#include <cstdint>
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

template <typename T, typename = void> struct has_std_out : std::false_type {};

template <typename T>
struct has_std_out<T, std::void_t<decltype(std::declval<std::ostream&>()
                                           << std::declval<T>())>>
    : std::true_type {};

template <typename T> class SharedPointer {
  private:
    std::atomic<size_t>* remain;
    T* t;

  public:
    using delFunc = void (*)(void*);
    delFunc selfDel;
    
    template <typename... Args>
    SharedPointer(Args&&... args) noexcept
        : remain(new std::atomic<size_t>(1)),
          t(new T(std::forward<Args>(args)...)){};
    ~SharedPointer() noexcept {
        int val = remain->fetch_sub(1);
        if (val == 1) {
            delete remain;
            delete t;
        } else {
            remain--;
            return;
        }
    }

    template <typename Y, typename Deleter> SharedPointer(std::nullptr_t ptr, Deleter del) {


    }

    SharedPointer(T* ptr) noexcept {
        remain = new std::atomic<size_t>(1);
        t = ptr;
    }

    SharedPointer(SharedPointer<T>& myptr) : remain(myptr.remain), t(myptr.t) {
        remain->fetch_add(1);
        cout << "copy constructor\n";
    }

    SharedPointer(SharedPointer<T>&& myptr)
        : remain(std::move(myptr.remain)), t(std::move(myptr.t)) {
        cout << "move constructor\n";
    }
    SharedPointer<T>& operator=(SharedPointer<T>& myptr) {
        cout << "copy assignment constructor\n";
        if (this == &myptr) {
            return *this;
        }
        myptr.remain->fetch_add(1);
        delete myptr;

        remain = myptr.remain;
        t = myptr.t;
        return *this;
    }

    size_t use_count() { return *remain; }

    friend std::ostream& operator<<(std::ostream& os,
                                    const SharedPointer<T>& myptr) {
        os << format("src addr : 0x{:p} remian addr : 0x{:p} cnt : {}\n",
                     static_cast<void*>(myptr.remain),
                     static_cast<void*>(myptr.t), (myptr.remain->load()));
        if constexpr (has_std_out<T>::value) {
            os << myptr.t;
        }
        return os;
    }

    void Release(SharedPointer<T> ptr) {
        *ptr.remain = 0;
        delete remain;
        delete this;
    }

    void reset(T* anotherPtr = nullptr) {
        SubtractOneCount();
        if (anotherPtr) {
            remain = new std::atomic<size_t>(1);
            t = anotherPtr;
        }
    }

    void SubtractOneCount() {
        int val = remain->fetch_sub(1);
        if (val == 1) {
            delete this;
        }
    }

    T* operator*() { return *t; }

    T* Get() { return this->t; }

    T* operator->() { return t; }
};

struct Foo {
    int a;
    double d;
    Foo() : a(0), d(0) {};
    Foo(int a_, double d_) : a(a_), d(d_) { std::cout << "Foo construct\n"; };

    template <typename T, std::enable_if<std::is_same_v<std::decay_t<T>, Foo>>>
    friend std::ostream& operator<<(std::ostream& os, T f) {
        os << format("a : {}, d : {}!!!!\n", f->a, f->d);
        return os;
    }

    virtual ~Foo() { std::cout << "Foo destruct\n"; };
};

struct Bar : Foo {
    float x;
    float y;
    Bar() : x(0), y(0) { cout << "Bar construct\n"; };
    Bar(float x_, float y_) : x(x_), y(y_) { cout << "Bar construct\n"; };
    virtual ~Bar() { cout << "Bar destruct\n"; };
};

int main() {
    auto temp = SharedPointer<Foo>(1, 3.3);

    std::cout << temp->a << "\n";
    cout << temp;
    {
        auto secondTemp(temp);
        cout << secondTemp;
        auto thirdTemp(std::move(secondTemp));
        cout << thirdTemp;
    }
    Foo* foo = new Bar(3314, 31.4f);
    temp.reset(foo);

    return 0;
}
