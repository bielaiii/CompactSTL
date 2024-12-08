#include <iostream>
#include <utility>
using std::cout;
using std::string;

namespace CompactSTL {
template <typename T, typename U> struct pair {
    T first;
    U second;
    pair() = delete;
    pair(T &&t, U &&u) : first(t), second(u) {};

    pair &operator=(pair<T, U> &others) {
        first  = others.first;
        second = others.second;
        return this;
    }

    // todo set proper restrict
    pair &operator=(pair<T, U> &&others) {
        first  = std::forward<T>(others.first);
        second = std::forward<U>(others.second);
        return this;
    }
};

template <typename T, typename U> pair<T, U> MakePair(T &&t, U &&u) {
    return pair(std::forward<T>(t), std::forward<U>(u));
}

} // namespace CompactSTL

int main() {
    using namespace CompactSTL;
    auto p1 = MakePair(1, 314.1231);
    cout << p1.first << "\n";
    cout << p1.second << "\n";

    return 0;
}