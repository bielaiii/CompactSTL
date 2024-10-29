#ifndef __HEADER_H__
#define __HEADER_H__



#include <iostream>
#include <type_traits>
#include<format>
using std::format;
struct Foo {
    int a;
    double d;
    Foo() : a(-2), d(0) {};
    Foo(int a_, double d_) : a(a_), d(d_) { std::cout << "Foo construct\n"; };

    template <typename T, std::enable_if<std::is_same_v<std::decay_t<T>, Foo>>>
    friend std::ostream& operator<<(std::ostream& os, T f) {
        os << format("a : {}, d : {}!!!!\n", f->a, f->d);
        return os;
    }


    friend std::ostream& operator<<(std::ostream& os, Foo& f) {
        os << format("a : {}, d : {}!!!!\n", f.a, f.d);
        return os;
    }

    virtual ~Foo() { std::cout << "Foo destruct\n"; };
};

struct Bar : Foo {
    float x;
    float y;
    Bar() : x(-2), y(0) { std::cout << "Bar construct\n"; };
    Bar(float x_, float y_) : x(x_), y(y_) { std::cout << "Bar construct\n"; };
    virtual ~Bar() { std::cout << "Bar destruct\n"; };
};
#endif