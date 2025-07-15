#include <format>
#include <iostream>

#include "array.hpp"

namespace CompactSTL {

using std::cout;
using std::print;
using std::println;
constexpr size_t default_size = 10;

template <typename Derived, typename T, size_t N> struct Expr {

    T operator[](size_t i) const noexcept {

        return static_cast<Derived const &>(*this)[i];
    }
    size_t size() const { return N; }
};

template <typename T, size_t N>
class ScalarWrapper : public Expr<ScalarWrapper<T, N>, T, N> {
    T value_;

public:
    explicit ScalarWrapper(T val) : value_(val) {
        //println("init value {}", value_);
    }
    ScalarWrapper(const ScalarWrapper &other) : value_(other.value_) {
        //println("copy ScalarWrapper {}", value_);
    }
    ScalarWrapper &operator=(const ScalarWrapper &other) {
        value_ = other.value_;
        //println("assign ScalarWrapper {}", value_);
        return *this;
    }
    T operator[](size_t i) const { return value_; }
};

template <typename op, typename LHS, typename RHS, typename T, size_t N>
class BinaryExpr : public Expr<BinaryExpr<op, LHS, RHS, T, N>, T, N> {
    LHS const lhs;
    RHS const rhs;

public:
    BinaryExpr(LHS const &l, RHS const &r) : lhs(l), rhs(r) {};
    T operator[](size_t i) const { return op::apply(lhs[i], rhs[i]); }
};

struct Add {
    template <typename T> static T apply(T a, T b) { return a + b; }
};

struct Sub {
    template <typename T> static T apply(T a, T b) { return a - b; }
};

struct Mul {
    template <typename T> static T apply(T a, T b) { return a * b; }
};

struct Div {
    template <typename T> static T apply(T a, T b) {
        if (b == 0) {
            throw std::runtime_error("division of zero");
        }

        return a / b;
    }
};

template <typename T, size_t N> struct A_Mat : Expr<A_Mat<T, N>, T, N> {
    std::array<T, N> data;
    A_Mat() = default;
    A_Mat(std::initializer_list<T> &init) {
        std::copy(init.begin(), init.end(), data.begin());
    }

    T &operator[](size_t i) noexcept { return data[i]; }

    T operator[](size_t i) const noexcept { return data[i]; }

    template <typename E> A_Mat &operator=(const Expr<E, T, N> const &e) {
        for (int i = 0; i < N; i++) {
            data[i] = e[i];
        }
        return *this;
    }

    void fill(T val) { data.fill(val); }

    void print() {
        for (auto d : data) {
            std::print("{},", d);
        }
        std::cout << "\n";
    }
    friend std::ostream &operator<<(std::ostream &os, A_Mat &mat) {

        for (auto d : mat.data) {
            std::print("{},", d);
        }
        std::cout << "\n";
        return os;
    }
};

/* add expr */
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator+(Expr<LHSExpr, T, N> const &lhs, Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Add, LHSExpr, RHSExpr, T, N>(
        static_cast<LHSExpr const &>(lhs), static_cast<RHSExpr const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator+(BinaryExpr<Add, LHSExpr, RHSExpr, T, N> const &lhs,
               Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Add, BinaryExpr<Add, LHSExpr, RHSExpr, T, N>,
                      Expr<RHSExpr, T, N>, T, N>(
        static_cast<BinaryExpr<Add, LHSExpr, RHSExpr, T, N> const &>(lhs),
        static_cast<RHSExpr const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator+(BinaryExpr<Add, LHSExpr, RHSExpr, T, N> const &lhs,
               T const &scalar) {
    return BinaryExpr<Add, BinaryExpr<Add, LHSExpr, RHSExpr, T, N>,
                      ScalarWrapper<T, N>, T, N>(
        static_cast<BinaryExpr<Add, LHSExpr, RHSExpr, T, N>>(lhs),
        static_cast<ScalarWrapper<T, N>>(scalar));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator+(T const &scalar,
               BinaryExpr<Add, LHSExpr, RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Add, ScalarWrapper<T, N>,
                      BinaryExpr<Add, LHSExpr, RHSExpr, T, N>, T, N>(
        static_cast<ScalarWrapper<T, N>>(scalar),
        static_cast<BinaryExpr<Add, LHSExpr, RHSExpr, T, N> const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator+(Expr<LHSExpr, T, N> const &lhs,
               BinaryExpr<Add, LHSExpr, RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Add, Expr<LHSExpr, T, N>,
                      BinaryExpr<Add, LHSExpr, RHSExpr, T, N>, T, N>(
        static_cast<Expr<LHSExpr, T, N> const &>(lhs),
        static_cast<BinaryExpr<Add, LHSExpr, RHSExpr, T, N> const &>(rhs));
}
template <typename LHSExpr, typename T, size_t N>
auto operator+(Expr<LHSExpr, T, N> const &lhs, T const &scalar) {
    return BinaryExpr<Add, LHSExpr, ScalarWrapper<T, N>, T, N>(
        static_cast<LHSExpr const &>(lhs), ScalarWrapper<T, N>(scalar));
}
template <typename RHSExpr, typename T, size_t N>
auto operator+(T const &scalar, Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Add, ScalarWrapper<T, N>, RHSExpr, T, N>(
        ScalarWrapper<T, N>(scalar), static_cast<RHSExpr const &>(rhs));
}

/* Sub expr */
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator-(Expr<LHSExpr, T, N> const &lhs, Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Sub, LHSExpr, RHSExpr, T, N>(
        static_cast<LHSExpr const &>(lhs), static_cast<RHSExpr const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator-(BinaryExpr<Sub, LHSExpr, RHSExpr, T, N> const &lhs,
               Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Sub, BinaryExpr<Sub, LHSExpr, RHSExpr, T, N>,
                      Expr<RHSExpr, T, N>, T, N>(
        static_cast<BinaryExpr<Sub, LHSExpr, RHSExpr, T, N> const &>(lhs),
        static_cast<RHSExpr const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator-(BinaryExpr<Sub, LHSExpr, RHSExpr, T, N> const &lhs,
               T const &scalar) {
    return BinaryExpr<Sub, BinaryExpr<Sub, LHSExpr, RHSExpr, T, N>,
                      ScalarWrapper<T, N>, T, N>(
        static_cast<BinaryExpr<Sub, LHSExpr, RHSExpr, T, N>>(lhs),
        static_cast<ScalarWrapper<T, N>>(scalar));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator-(T const &scalar,
               BinaryExpr<Sub, LHSExpr, RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Sub, ScalarWrapper<T, N>,
                      BinaryExpr<Sub, LHSExpr, RHSExpr, T, N>, T, N>(
        static_cast<ScalarWrapper<T, N>>(scalar),
        static_cast<BinaryExpr<Sub, LHSExpr, RHSExpr, T, N> const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator-(Expr<LHSExpr, T, N> const &lhs,
               BinaryExpr<Sub, LHSExpr, RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Sub, Expr<LHSExpr, T, N>,
                      BinaryExpr<Sub, LHSExpr, RHSExpr, T, N>, T, N>(
        static_cast<Expr<LHSExpr, T, N> const &>(lhs),
        static_cast<BinaryExpr<Sub, LHSExpr, RHSExpr, T, N> const &>(rhs));
}
template <typename LHSExpr, typename T, size_t N>
auto operator-(Expr<LHSExpr, T, N> const &lhs, T const &scalar) {
    return BinaryExpr<Sub, LHSExpr, ScalarWrapper<T, N>, T, N>(
        static_cast<LHSExpr const &>(lhs), ScalarWrapper<T, N>(scalar));
}
template <typename RHSExpr, typename T, size_t N>
auto operator-(T const &scalar, Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Sub, ScalarWrapper<T, N>, RHSExpr, T, N>(
        ScalarWrapper<T, N>(scalar), static_cast<RHSExpr const &>(rhs));
}

/* mul expr */
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator*(Expr<LHSExpr, T, N> const &lhs, Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Mul, LHSExpr, RHSExpr, T, N>(
        static_cast<LHSExpr const &>(lhs), static_cast<RHSExpr const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator*(BinaryExpr<Mul, LHSExpr, RHSExpr, T, N> const &lhs,
               Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Mul, BinaryExpr<Mul, LHSExpr, RHSExpr, T, N>,
                      Expr<RHSExpr, T, N>, T, N>(
        static_cast<BinaryExpr<Mul, LHSExpr, RHSExpr, T, N> const &>(lhs),
        static_cast<RHSExpr const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator*(BinaryExpr<Mul, LHSExpr, RHSExpr, T, N> const &lhs,
               T const &scalar) {
    return BinaryExpr<Mul, BinaryExpr<Mul, LHSExpr, RHSExpr, T, N>,
                      ScalarWrapper<T, N>, T, N>(
        static_cast<BinaryExpr<Mul, LHSExpr, RHSExpr, T, N>>(lhs),
        static_cast<ScalarWrapper<T, N>>(scalar));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator*(T const &scalar,
               BinaryExpr<Mul, LHSExpr, RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Mul, ScalarWrapper<T, N>,
                      BinaryExpr<Mul, LHSExpr, RHSExpr, T, N>, T, N>(
        static_cast<ScalarWrapper<T, N>>(scalar),
        static_cast<BinaryExpr<Mul, LHSExpr, RHSExpr, T, N> const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator*(Expr<LHSExpr, T, N> const &lhs,
               BinaryExpr<Mul, LHSExpr, RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Mul, Expr<LHSExpr, T, N>,
                      BinaryExpr<Mul, LHSExpr, RHSExpr, T, N>, T, N>(
        static_cast<Expr<LHSExpr, T, N> const &>(lhs),
        static_cast<BinaryExpr<Mul, LHSExpr, RHSExpr, T, N> const &>(rhs));
}
template <typename LHSExpr, typename T, size_t N>
auto operator*(Expr<LHSExpr, T, N> const &lhs, T const &scalar) {
    return BinaryExpr<Mul, LHSExpr, ScalarWrapper<T, N>, T, N>(
        static_cast<LHSExpr const &>(lhs), ScalarWrapper<T, N>(scalar));
}
template <typename RHSExpr, typename T, size_t N>
auto operator*(T const &scalar, Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Mul, ScalarWrapper<T, N>, RHSExpr, T, N>(
        ScalarWrapper<T, N>(scalar), static_cast<RHSExpr const &>(rhs));
}

/* div expr */
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator/(Expr<LHSExpr, T, N> const &lhs, Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Div, LHSExpr, RHSExpr, T, N>(
        static_cast<LHSExpr const &>(lhs), static_cast<RHSExpr const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator/(BinaryExpr<Div, LHSExpr, RHSExpr, T, N> const &lhs,
               Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Div, BinaryExpr<Div, LHSExpr, RHSExpr, T, N>,
                      Expr<RHSExpr, T, N>, T, N>(
        static_cast<BinaryExpr<Div, LHSExpr, RHSExpr, T, N> const &>(lhs),
        static_cast<RHSExpr const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator/(BinaryExpr<Div, LHSExpr, RHSExpr, T, N> const &lhs,
               T const &scalar) {
    return BinaryExpr<Div, BinaryExpr<Div, LHSExpr, RHSExpr, T, N>,
                      ScalarWrapper<T, N>, T, N>(
        static_cast<BinaryExpr<Div, LHSExpr, RHSExpr, T, N>>(lhs),
        static_cast<ScalarWrapper<T, N>>(scalar));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator/(T const &scalar,
               BinaryExpr<Div, LHSExpr, RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Div, ScalarWrapper<T, N>,
                      BinaryExpr<Div, LHSExpr, RHSExpr, T, N>, T, N>(
        static_cast<ScalarWrapper<T, N>>(scalar),
        static_cast<BinaryExpr<Div, LHSExpr, RHSExpr, T, N> const &>(rhs));
}
template <typename LHSExpr, typename RHSExpr, typename T, size_t N>
auto operator/(Expr<LHSExpr, T, N> const &lhs,
               BinaryExpr<Div, LHSExpr, RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Div, Expr<LHSExpr, T, N>,
                      BinaryExpr<Div, LHSExpr, RHSExpr, T, N>, T, N>(
        static_cast<Expr<LHSExpr, T, N> const &>(lhs),
        static_cast<BinaryExpr<Div, LHSExpr, RHSExpr, T, N> const &>(rhs));
}
template <typename LHSExpr, typename T, size_t N>
auto operator/(Expr<LHSExpr, T, N> const &lhs, T const &scalar) {
    return BinaryExpr<Div, LHSExpr, ScalarWrapper<T, N>, T, N>(
        static_cast<LHSExpr const &>(lhs), ScalarWrapper<T, N>(scalar));
}
template <typename RHSExpr, typename T, size_t N>
auto operator/(T const &scalar, Expr<RHSExpr, T, N> const &rhs) {
    return BinaryExpr<Div, ScalarWrapper<T, N>, RHSExpr, T, N>(
        ScalarWrapper<T, N>(scalar), static_cast<RHSExpr const &>(rhs));
}

} // namespace CompactSTL
