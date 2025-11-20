#include "function.hpp"
#include <iostream>
#include <type_traits>
#include <utility>

namespace CompactSTL {

template <size_t IDX, typename T>
struct TupleElement {
    T value;
    TupleElement() = default;
    TupleElement(const T &t_) noexcept(noexcept(T())) : value(t_) {};
    TupleElement(T &&t_) noexcept(noexcept(T()))
        : value(std::move(t_)) {};
};
template <size_t IDX, typename... Tys>
struct TupleImpl;

template <size_t STOP_HERE>
struct TupleImpl<STOP_HERE> {};

template <size_t IDX, typename T, typename... RES>
struct TupleImpl<IDX, T, RES...> : TupleElement<IDX, T>,
                                   TupleImpl<IDX + 1, RES...> {
    using base_hierachy = TupleImpl<IDX + 1, RES...>;
    using Element_      = TupleElement<IDX, T>;

    TupleImpl() = default;

    TupleImpl(const T &t, const RES &...res)
        : Element_(t), base_hierachy(res...) {};
};

template <typename... T>
struct Tuple : TupleImpl<0, T...> {
    using base_ = TupleImpl<0, T...>;
    Tuple()     = default;

    Tuple(const T &...t) : base_(t...) {};
};

template <size_t IDX, typename T, typename... RES>
struct GetType {
    using type = GetType<IDX - 1, RES...>::type;
};

template <typename T, typename... RES>
struct GetType<0, T, RES...> {
    using type = T;
};

template <size_t IDX, typename T, typename... RES>
TupleElement<IDX, T> &GetElement(TupleImpl<IDX, T, RES...> &tuple_) {
    return tuple_;
}

template <size_t IDX, typename T, typename... RES>
const TupleElement<IDX, T> &GetElement(const TupleImpl<IDX, T, RES...> &tuple_) {
    return tuple_;
}

template <size_t N, typename... Tuple_type>
auto &Get(CompactSTL::Tuple<Tuple_type...> &tuple_) {
    return GetElement<N>(tuple_).value;
}

template <size_t N, typename... Tuple_type>
const auto &Get(const CompactSTL::Tuple<Tuple_type...> &tuple_) {
    return GetElement<N>(tuple_).value;
}

} // namespace CompactSTL
