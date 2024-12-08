#ifndef __HEADER_H__
#define __HEADER_H__

#include <format>
#include <iostream>
#include <type_traits>
using std::format;
struct Foo {
    int a;
    double d;
    Foo() : a(1), d(2) { std::cout << "Foo is default construct\n";};
    Foo(int a_, double d_) : a(a_), d(d_) { std::cout << "Foo construct\n"; };

    template <typename T, std::enable_if<std::is_same_v<std::decay_t<T>, Foo>>>

    friend std::ostream &operator<<(std::ostream &os, const Foo &foo) {
        os << format("Foo : a : {}, d : {}!!!!\n", foo.a, foo.d);
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
    friend std::ostream &operator<<(std::ostream &os, const Bar &bar) {
        os << format("Bar : a : {}, d : {}!!!!\n", bar.a, bar.d);
        return os;
    }
};
#endif