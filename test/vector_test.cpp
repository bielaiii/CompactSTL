#include "vector.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <print>
#include <string>
#include <type_traits>
#include <vector>

using namespace CompactSTL;
struct MyType {
    int value;
    std::string name;
    double currency;
};

template <typename T> void func(T &vec) {
    vec.emplace_back(1, "noname", 100.0);
    println("name : {} val : {} currency : {}", vec.begin()->name,
            vec.begin()->value, vec.begin()->currency);
}
void test_basic() {
    vector<int> v;
    assert(v.size() == 0);
    // std::println("size : {}, capacity : {}", v.size(), v.capacity());
    assert(v.capacity() > 0);
    v.emplace_back(1);
    v.emplace_back(2);
    v.emplace_back(3);
    assert(v.size() == 3);
    assert(v[0] == 1 && v[1] == 2 && v[2] == 3);
    v.pop_back();
    assert(v.size() == 2);
    v.clear();
    assert(v.size() == 0);
}

void test_at_and_exception() {
    vector<int> v;
    v.emplace_back(10);
    try {
        v.at(1);
        assert(false); // 应该抛出异常
    } catch (const std::runtime_error &) {
        // 正常
    }
}

void test_front_back() {
    vector<int> v;
    v.emplace_back(42);
    v.emplace_back(43);
    assert(v.front() == 42);
    assert(v.back() == 43);
}

void test_iterator_and_std_algorithm() {
    vector<int> v;
    for (int i = 0; i < 5; ++i)
        v.emplace_back(i);
    // std::accumulate
    int sum = std::accumulate(v.begin(), v.end(), 0);
    assert(sum == 0 + 1 + 2 + 3 + 4);
    // std::find
    auto it = std::find(v.begin(), v.end(), 3);
    assert(it != v.end() && *it == 3);
    // std::sort
    std::reverse(v.begin(), v.end());
    assert(v[0] == 4 && v[4] == 0);
}

void test_copy_and_move() {
    vector<int> v1;
    v1.emplace_back(1);
    v1.emplace_back(2);
    vector<int> v2 = v1; // 拷贝构造
    assert(v2.size() == 2 && v2[0] == 1 && v2[1] == 2);
    vector<int> v3 = std::move(v1); // 移动构造
    assert(v3.size() == 2 && v3[0] == 1 && v3[1] == 2);
}

void test_custom_type() {
    vector<MyType> v;
    v.emplace_back(MyType{1, "a", 1.1});
    v.emplace_back(MyType{2, "b", 2.2});
    assert(v[0].name == "a" && v[1].currency == 2.2);
}

struct base {
    int a;
};
struct Derived : public base {
    float v;
    ~Derived()
    {
        cout << "call my destructor\n";
    }
};

int main() {
    test_basic();
    test_at_and_exception();
    test_front_back();
    test_iterator_and_std_algorithm();
    test_copy_and_move();
    test_custom_type();
    std::cout << "All tests passed!" << std::endl;

    return 0;
}