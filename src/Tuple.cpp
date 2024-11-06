#include <iostream>
#include <utility>

namespace CompactSTL {
template <typename... Types> struct Tuple;

template <typename T> struct Tuple<T> {
    T t;
    explicit Tuple(T &&t) : t(std::forward<T>(t)) {};
    T &get() { return t; }
    const T &get() const { return t; }

    template<size_t = 0>
    T &GetElement() { return t; }
};

template <typename T, typename... Args>
struct Tuple<T, Args...> : Tuple<Args...> {
    T t;

    Tuple() = delete;

    explicit Tuple(T &&t, Args &&...args)
        : Tuple<Args...>(std::forward<Args>(args)...), t(std::forward<T>(t)) {}

    T &Get() { return t; }
    const T &Get() const { return t; }

    template <size_t N> auto &GetElement() {
        if constexpr (N == 0) {
            return t;
        } else {
            return Tuple<Args...>::template GetElement<N - 1>();
        }
    }
};

template <size_t n, typename... Args> auto &Get(Tuple<Args...> &tu) {
    return tu.template GetElement<n>();
}

} // namespace CompactSTL

int main() {
    using std::cout;
    using std::string;
    using namespace CompactSTL;
    Tuple<int, double, string> tu(1, 3.14, "hello,world");
    cout << Get<0>(tu) << "\n";
    cout << Get<1>(tu) << "\n";
    cout << Get<2>(tu) << "\n";
    return 0;
}