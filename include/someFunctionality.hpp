#include <iostream>
#include <map>
#include <set>
#include <string>
#include <type_traits>

namespace CompactSTL {

template <typename T> struct RemoveConst {
    using mytype = T;
};
template <typename T> struct RemoveConst<const T> {
    using mytype = T;
};
template <typename T> struct RemoveVolatile {
    using mytype = T;
};
template <typename T> struct RemoveVolatile<volatile T> {
    using mytype = T;
};

template <typename T> struct RemoveCV {
    using mytype =
        typename RemoveVolatile<typename RemoveConst<T>::mytype>::mytype;
};

template <typename T> struct AddPointer {
    using mytype = T *;
};

template <typename T> struct RemovePtr {
    using mytype = T;
};

template <typename T> struct RemovePtr<T *> {
    using mytype = typename RemovePtr<T>::mytype;
};
}; // namespace CompactSTL
