#include <iostream>
#include <map>
#include <set>
#include <string>
#include <type_traits>


namespace CompactSTL {

template<typename T>
struct RemoveConst {
    using mytype = T;
};
template<typename T>
struct RemoveConst<const T> {
    using mytype = T;
};
template<typename T>
struct RemoveVolatile {
    using mytype = T;
};
template<typename T>
struct RemoveVolatile<volatile T> {
    using mytype = T;
};

template<typename T>
struct RemoveCV{
    using mytype = RemoveVolatile<typename RemoveConst<T>::mytype>::mytype;
};

template<typename T>
struct AddPointer {
    using mytype = T*;
};

template<typename T>
struct RemovePtr {
    using mytype = T;
};

template<typename T>
struct RemovePtr<T*> {
   using mytype = RemovePtr<T>::mytype; 
};
};


template<typename T>
void foo(T t) {
    using deducedType = typename CompactSTL::RemoveCV<T>::mytype;
    static_assert(std::is_same_v<deducedType, int>, "is same type");
    std::cout << "good\n";
}

template<typename T>
void test1(T t) {
    using removePtrType = typename CompactSTL::RemovePtr<T>::mytype;
    static_assert(std::is_same_v<removePtrType, int>, "should not see");
}

int main() {
    int const volatile a = 10;
    foo(a);
    const int volatile b = 10;
    foo(b);
    int *** c ;
    int ** d ;
    int *e ;
    test1(c);
    test1(d);
    test1(e);
    return 0;
}
