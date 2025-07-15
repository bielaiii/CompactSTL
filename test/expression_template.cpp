#include "iterator.hpp"
#include <array>
#include <cstddef>
#include <format>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <ostream>
#include <print>
#include <stdexcept>
#include <utility>
#include"expression_template.hpp"

int main() {
    using namespace CompactSTL;
    using namespace CompactSTL;
    using std::cout;
    using test_array = A_Mat<int, default_size>;
    auto init_lst    = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    test_array arr1  = init_lst;
    test_array arr2;
    test_array arr3;
    arr2.fill(7);
    arr3.fill(4);
    test_array answer;
    answer = (10 * arr3 + (arr1 + arr2)) / 2;
    cout << answer;

    return 0;
}