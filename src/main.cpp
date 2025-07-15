#include <compare>
#include <functional>
#include <iostream>

#include <coroutine>
#include <memory>
#include <print>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>
// using namespace std;
using std::print;
using std::println;
#include "vector.hpp"
#include <array>
#include<array.hpp>
struct AAA {
    struct {
        float v;
        unsigned long ll;
    } a;
    // static iOt value;
};

int main() {
    using namespace CompactSTL;
    Array<int, 8> arr;
    arr[0] = 0;
    arr[1] = 1;
    arr[2] = 2;
    arr[3] = 3;
    arr[4] = 4;
    arr[5] = 5;
    arr[6] = 6;
    arr[7] = 7;
    //std::array<int, 8> stdarr;
    std::vector<int> stdvt;
    int a = 10;
    stdvt.push_back(a);
    //stdvt.begin();
    for (auto c = arr.rbegin(); c != arr.rend(); ++c) {
        println("{}", *c);
    
    }
    //std::vector<int> std
    arr[0] = 1;
    return 0;
}
