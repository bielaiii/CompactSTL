#include <iostream>
#include <type_traits>
#include <utility>
using std::cout;
using std::string;

namespace CompactSTL {
template <typename T, typename U> struct pair {
  T first;
  U second;

  friend void
  swap(CompactSTL::pair<T, U> &lhs,
       CompactSTL::pair<T, U> &rhs) noexcept(std::is_nothrow_swappable_v<T> &&
                                             std::is_nothrow_swappable_v<U>) {
    std::swap(lhs.first, rhs.first);
    std::swap(lhs.second, rhs.second);
  }

  pair(const CompactSTL::pair<T, U> &other) noexcept
      : first(other.first), second(other.second) {}

  pair() = default;
  pair(T &&t, U &&u) : first(std::forward<T>(t)), second(std::forward<U>(u)){};

  pair(pair<T, U> &&other) noexcept
      : first(std::move(other.first)), second(std::move(other.second)){};

  pair &operator=(pair<T, U> &others) {
    first = others.first;
    second = others.second;
    return *this;
  }

  // todo set proper restrict
  pair &operator=(pair<T, U> &&others) {
    first = std::forward<T>(others.first);
    second = std::forward<U>(others.second);
    return *this;
  }
};

template <typename T, typename U> pair<T, U> MakePair(T &&t, U &&u) {
  return pair(std::forward<T>(t), std::forward<U>(u));
}

} // namespace CompactSTL
