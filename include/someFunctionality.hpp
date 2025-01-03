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
struct RemoveLeftReference {
    using mytype = T;
};

template<typename T>
struct RemoveLeftReference<T&> {
    using mytype = typename RemoveLeftReference<T>::mytype;
};
template<typename T>
struct RemoveRightReference {
    using mytype = T;
};

template<typename T>
struct RemoveRightReference<T&&> {
    using mytype = typename RemoveRightReference<T>::mytype;
};
template<typename T>
struct RemoveReference {
    using mytype = typename RemoveLeftReference<typename RemoveRightReference<T>::mytype>::mytype;
};


template <typename T, typename = void> struct IsPtr : std::true_type {};

template <typename T>
    struct IsPtr < T,
    std::enable_if<!std::is_same_v<T, typename CompactSTL::RemovePtr<T>::mytype>>>
    : std::false_type {};
template <typename T>
struct is_function_ptr : std::false_type {};

template <typename R, typename ... Args>
struct is_function_ptr<R(*)(Args...)>
    : std::true_type {};

template <typename T, typename = void>
struct is_callable_struct : std::false_type {};

template <typename T>
struct is_callable_struct<T,
                          std::void_t<std::enable_if_t<std::is_class<T>::value>,
                                      decltype(std::declval<T>()())>>
    : std::true_type {};

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
}; // namespace CompactSTL
