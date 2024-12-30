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

template<typename T>
struct RemoveReference {
    using mytype = T;
};



template<typename T>
struct RemoveLeftReference<T&> {
    using mytype = typename RemoveReference<T>::mytype;
};


template<typename T>
struct RemoveRightReference<T&> {
    using mytype = typename RemoveReference<T>::mytype;
};



template <typename T, typename = void> struct IsPtr : std::true_type {};

template <typename T>
    struct IsPtr < T,
    std::enable_if<!std::is_same_v<T, typename CompactSTL::RemovePtr<T>::mytype>>>
    : std::false_type {};

}; // namespace CompactSTL
