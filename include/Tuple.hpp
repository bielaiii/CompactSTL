#include "function.hpp"
#include <iostream>
#include <type_traits>
#include <utility>

namespace CompactSTL {

template <typename... Types>
struct TupleImpl;

template <typename T>
struct TupleImpl<T> {
    T t;
    TupleImpl() = default;
    template <typename T_, typename dummy = std::enable_if_t<
                               std::is_constructible_v<T, T_ &&>>>
    TupleImpl(T_ &&t) noexcept : t(std::forward<T_>(t)){};
    T &get() { return t; }
    const T &get() const { return t; }


    template <size_t = 0>
    T &GetElement() & {
        return t;
    }

    template <size_t = 0>
    const T &GetElement() const & {
        return t;
    }

    template <size_t = 0>
    T &&GetElement() && {
        return std::move(t);
    }

    template <size_t = 0>
    const T &&GetElement() const && {
        return std::move(t);
    }
};

template <std::size_t N, typename... Args>
struct GetTupleType;


template <size_t N, typename T, typename... Args>
struct GetTupleType<N, T, Args...> {
    using Type_ = typename GetTupleType<N - 1, Args...>::Type_;
};

template <typename T, typename... Args>
struct GetTupleType<0, T, Args...> {
    using Type_ = std::remove_cvref_t<T>;
};

template <typename T>
struct GetTupleType<0, T> {
    using Type_ = std::remove_cvref_t<T>;
};
template <typename T, typename... Args>
struct TupleImpl<T, Args...> : TupleImpl<Args...> {
    T t;

    TupleImpl() = delete;

    
    using Base = TupleImpl<Args...>;
    using Base::GetElement;

    T &Get() { return t; }
    const T &Get() const { return t; }

    template <typename T_, typename... Tys,
              typename = std::enable_if_t<std::is_constructible_v<T, T_ &&>>>
    TupleImpl(T_ &&v, Tys &&...rest) noexcept(
        std::is_nothrow_constructible_v<T, T_ &&> &&
        (std::is_nothrow_constructible_v<Args, Tys &&> && ...))
        : Base(std::forward<Tys>(rest)...), t(std::forward<T_>(v)) {}
    
    
    template <size_t N>
    decltype(auto) GetElement() & {
        if constexpr (N == 0) {
            return t;
        } else {
            return Base::template GetElement<N - 1>();
        }
    }

    template <size_t N >
    decltype(auto) GetElement() const & {
        if constexpr (N == 0) {
            return t;
        } else {
            return Base::template GetElement<N - 1>();
        }
    }

    template <size_t N>
    decltype(auto) GetElement() && {
        if constexpr (N == 0) {
            return std::move(t);
        } else {
            return static_cast<Base &&>(*this).template GetElement<N - 1>();
        }
    }

    template <size_t N>
    decltype(auto) GetElement() const && {
        if constexpr (N == 0) {
            return std::move(t);
        } else {
            return static_cast<const Base &&>(*this)
                .template GetElement<N - 1>();
        }
    }
};

template <typename... T>
struct Tuple {
    TupleImpl<T...> baseTuple;
    template <typename... Args>
    Tuple(Args &&...arg) noexcept : baseTuple(std::forward<Args>(arg)...){};

    Tuple(const Tuple &)            = default;
    Tuple(Tuple &&)                 = default;
    Tuple &operator=(const Tuple &) = default;
    Tuple &operator=(Tuple &&)      = default;
};

template<size_t N, typename... Args>
struct GetHelper {
    
    template<size_t C, typename Cur, typename...  Res>
    static auto& GetI(CompactSTL::Tuple<Args...> & t) {
        return GetHelper<N  - 1, Res...>::GetI(t);
    }
    
    template<size_t C, typename Cur, typename...  Res>
    static const auto& GetI(CompactSTL::Tuple<Args...> & t) {
        return GetHelper<N  - 1, Res...>::GetI(t);
    }


};

#if 0
template <size_t n, typename... Args>
typename GetTupleType<n, Args...>::Type_ Get(Tuple<Args...> &lr_tu) noexcept {
    return lr_tu.baseTuple.template GetElement<n>();
}

template <size_t n, typename... Args>
typename GetTupleType<n, Args...>::Type_ Get(const Tuple<Args...> &lc_tu) noexcept {
    return lc_tu.baseTuple.template GetElement<n>();
}

template <size_t n, typename... Args>
GetTupleType<n, Args...>::Type_&& Get(const Tuple<Args...> &&rc_ctu) noexcept {
    return static_cast<const Tuple<Args...> &&>(rc_ctu)
        .baseTuple.template GetElement<n>();
}

template <size_t n, typename... Args>
GetTupleType<n, Args...>::Type_&& Get(Tuple<Args...> &&r_tu) noexcept {
    return static_cast<Tuple<Args...> &&>(r_tu)
        .baseTuple.template GetElement<n>();
}
#endif


// 左值版本：返回 T&
template <size_t n, typename... Args>
decltype(auto) Get(Tuple<Args...> &t) noexcept {
    return t.baseTuple.template GetElement<n>();
}

// const 左值版本：返回 const T&
template <size_t n, typename... Args>
decltype(auto) Get(const Tuple<Args...> &t) noexcept {
    return t.baseTuple.template GetElement<n>();
}

// 右值版本：返回 T&&
template <size_t n, typename... Args>
decltype(auto) Get(Tuple<Args...> &&t) noexcept {
    return static_cast<Tuple<Args...> &&>(t).baseTuple.template GetElement<n>();
}

// const 右值版本：返回 const T&&（较少用，但保持完整）
template <size_t n, typename... Args>
decltype(auto) Get(const Tuple<Args...> &&t) noexcept {
    return static_cast<const Tuple<Args...> &&>(t).baseTuple.template GetElement<n>();
}

} // namespace CompactSTL
