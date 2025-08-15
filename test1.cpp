#include <cstddef>
#include <iostream>
#include <iterator>
#include <print>
#include <type_traits>
#include <vector>
using namespace std;

template <size_t N> constexpr size_t fib(size_t) {
    if constexpr (N == 0) {
        return 1;
    } else if constexpr (N == 1) {
        return 1;
    } else {
        return fib<N - 1>(1) + fib<N - 2>(1);
    }
}

template <size_t N> struct fibst {
    static inline size_t value = fibst<N - 1>::value + fibst<N - 2>::value;
};

template <> struct fibst<1> {
    static inline size_t value = 1;
};
template <> struct fibst<0> {
    static inline size_t value = 1;
};


template<typename T>
concept my_is_integral = is_integral_v<T> && is_floating_point_v<T>;

template <my_is_integral T>
    //requires is_integral_v<T>
T foo(T t) {
    return t + 1;
}
int main() {
    cout << fib<0>(1) << std::endl;
    cout << fib<1>(1) << std::endl;
    cout << fib<2>(1) << std::endl;
    cout << fib<3>(1) << std::endl;
    cout << fibst<0>::value << std::endl;
    cout << fibst<1>::value << std::endl;
    cout << fibst<2>::value << std::endl;
    cout << fibst<3>::value << std::endl;

    return 0;
};