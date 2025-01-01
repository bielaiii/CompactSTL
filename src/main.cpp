#include <iostream>

#include "header.h"
#include "someFunctionality.hpp"
#include <type_traits>
#include <utility>

namespace CompactSTL {
template <typename T, typename = void>
struct is_function_ptr : std::false_type {
};

template <typename T>
struct is_function_ptr<T, std::void_t<
    std::conjunction<decltype(std::is_function_v<T>),
                                      decltype(std::is_pointer_v<T>)>>>
    : std::true_type {
};

template <typename T, typename = void>
struct is_callable_struct : std::false_type {
};

template <typename T>
struct is_callable_struct<T,
                          std::void_t<std::enable_if_t<std::is_class<T>::value>,
                                      decltype(std::declval<T>()())>>
    : std::true_type {
};

template <typename T, typename = void>
struct is_callable_object : std::false_type {};

template <typename T>
struct is_callable_object<
    T, std::enable_if_t<
           std::disjunction<
               typename CompactSTL::is_function_ptr<T>::value,
               typename CompactSTL::is_callable_struct<T>::value>::value,
           void>> : std::true_type {
    using type = T;
};
} // namespace CompactSTL

struct CalFun {
    void operator()() { std::cout << "CalFun\n"; };
};

void Delfunc(Foo *foo) {
    std::cout << "Delete Foo\n";
    delete foo;
}

int main() {
    using std::cout;
    Foo *foo;
    Bar bar;
    cout << std::is_function_v<Foo> << std::endl;
    cout << std::is_function_v<Foo*> << std::endl;
    /* std::cout << CompactSTL::is_callable_struct<CalFun>::value << "\n";
    std::cout << CompactSTL::is_callable_struct<Foo>::value << "\n";
    std::cout << CompactSTL::is_callable_struct<Bar>::value << "\n";

    std::cout << CompactSTL::is_function_ptr<decltype(Delfunc)>::value << "\n";
    std::cout << "shoudle not " << CompactSTL::is_function_ptr<decltype(foo)>::value << "\n";

    auto lbd = []() { cout << "hello,world\n"; };

    std::cout << CompactSTL::is_callable_object<CalFun>::value << "\n";
    std::cout << CompactSTL::is_callable_object<Foo>::value << "\n";
    std::cout << CompactSTL::is_callable_object<Bar>::value << "\n";
    std::cout << CompactSTL::is_callable_object<decltype(lbd)>::value << "\n";
    std::cout << "del func ptr "
              << CompactSTL::is_function_ptr<decltype(Delfunc)>::value << "\n";
    std::cout << "del func ptr obj "
              << CompactSTL::is_callable_struct<decltype(Delfunc)>::value
              << "\n"; */
    //cout << std::void_t<CompactSTL::is_function_ptr<CalFun>::value>::value << "\n";
    return 0;
}