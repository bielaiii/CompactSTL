#include <algorithm> // std::copy
#include <barrier>
#include <cstddef> // std::size_t
#include <cstdint>
#include <format>
#include <iostream>
#include <memory>
#include <ostream>
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

template <typename T> class MySharePointer {
  private:
    size_t* remain;
    T* t;

  public:
    template <typename... Args>
    MySharePointer(Args&&... args) noexcept
        : remain(new size_t(1)), t(new T(std::forward<Args>(args)...)){};
    ~MySharePointer() noexcept {
        (*remain)--;
        if ((*remain) == 0) {
            delete remain;
            delete t;
        } else {
            remain--;
            return;
        }
    }

    template <typename Y, typename Deleter>
    MySharePointer() {
        

    }

    MySharePointer(T* ptr) noexcept {
        remain = new size_t(1);
        t = ptr;
    }

    MySharePointer(MySharePointer<T>& myptr)
        : remain(myptr.remain), t(myptr.t) {
        (*remain)++;
        cout << "copy constructor\n";
    }

    MySharePointer(MySharePointer<T>&& myptr)
        : remain(std::move(myptr.remain)), t(std::move(myptr.t)) {
        cout << "move constructor\n";
    }
    MySharePointer<T>& operator=(MySharePointer<T>& myptr) {
        cout << "copy assignment constructor\n";
        if (this == &myptr) {
            return *this;
        }
        myptr.remain++;
        remain--;
        if ((*remain) == 0) {
            delete remain;
            delete t;
        }
        remain = myptr.remain;
        t = myptr.t;
        return *this;
    }

    size_t use_count() { return *remain; }

    friend std::ostream& operator<<(std::ostream& os,
                                    const MySharePointer<T>& myptr) {
        os << format("src addr : 0x{:x} remian addr : 0x{:x} cnt : {}\n",
                     reinterpret_cast<std::uintptr_t>(myptr.remain),
                     reinterpret_cast<std::uintptr_t>(myptr.t),
                     *(myptr.remain));
        if constexpr (has_std_out<T>::value) {
            os << myptr.t;
        }
        return os;
    }

    void Release(MySharePointer<T> ptr) {
        *ptr.remain = 0;
        delete remain;
        delete this;
    }

    void reset(T* anotherPtr = nullptr) {
        SubtractOneCount();
        if (anotherPtr) {
            remain = new size_t(1);
            t = anotherPtr;
        }
    }

    void SubtractOneCount() {
        (*remain)--;
        if (*remain == 0) {
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
    Foo(int a_, double d_) : a(a_), d(d_) { std::cout << "Foo construct\n"; };

    template <typename T, std::enable_if<std::is_same_v<std::decay_t<T>, Foo>>>
    friend std::ostream& operator<<(std::ostream& os, T f) {
        os << format("a : {}, d : {}!!!!\n", f->a, f->d);
        return os;
    }

    ~Foo() { std::cout << "Foo destruct\n"; };
};

struct Bar {
    float x;
    float y;
    Bar(float x_, float y_) : x(x_), y(y_){};
};

int main() {
    auto temp = MySharePointer<Foo>(1, 3.3);

    std::cout << temp->a << "\n";
    cout << temp;
    {
        auto secondTemp(temp);
        cout << secondTemp;
        auto thirdTemp(std::move(secondTemp));
        cout << thirdTemp;
    }
    auto bar = new Foo(3314, 31.4f);
    temp.reset(bar);

    return 0;
}
