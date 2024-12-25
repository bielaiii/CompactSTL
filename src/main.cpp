#include <iostream>

#include "header.h"
#include "someFunctionality.hpp"
#include <type_traits>
#include <utility>

namespace CompactSTL {
template <typename T, typename = void>
struct is_function_ptr : std::false_type {};

template <typename T>
struct is_function_ptr<T, std::void_t<decltype(std::is_function_v<T>), decltype(std::is_pointer_v<T>)>>
    : std::true_type {};

template <typename T, typename = void>
struct is_callable_struct : std::false_type {};

template <typename T>
struct is_callable_struct<T,
                          std::void_t<std::enable_if_t<std::is_class<T>::value>,
                                      decltype(std::declval<T>()())>>
    : std::true_type {};

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
    std::cout << CompactSTL::is_callable_struct<CalFun>::value << "\n";
    std::cout << CompactSTL::is_callable_struct<Foo>::value << "\n";
    std::cout << CompactSTL::is_callable_struct<Bar>::value << "\n";

    std::cout << CompactSTL::is_function_ptr<decltype(Delfunc)>::value << "\n";
    std::cout << CompactSTL::is_function_ptr<decltype(foo)>::value << "\n";
    return 0;
}