#include <cassert>
#include <iostream>
#include <string>

#include "Pair.hpp"

using namespace CompactSTL;

static void test_basic_pair() {
    pair<int, std::string> p(42, "hello");
    assert(p.first == 42);
    assert(p.second == "hello");
}

static void test_make_pair() {
    auto p = MakePair(10, 3.14);
    assert(p.first == 10);
    assert(p.second == 3.14);
}

static void test_copy_assignment() {
    pair<int, int> p1(1, 2);
    pair<int, int> p2(3, 4);
    
    p1 = p2;
    assert(p1.first == 3);
    assert(p1.second == 4);
}

static void test_move_assignment() {
    pair<int, std::string> p1(10, "original");
    pair<int, std::string> p2(20, "moved");
    
    p1 = std::move(p2);
    assert(p1.first == 20);
    assert(p1.second == "moved");
}

static void test_different_types() {
    pair<std::string, double> p("pi", 3.14159);
    assert(p.first == "pi");
    assert(p.second == 3.14159);
}

static void test_nested_pair() {
    pair<int, pair<int, int>> p(1, pair<int, int>(2, 3));
    assert(p.first == 1);
    assert(p.second.first == 2);
    assert(p.second.second == 3);
}

int main() {
    try {
        test_basic_pair();
        test_make_pair();
        test_copy_assignment();
        test_move_assignment();
        test_different_types();
        test_nested_pair();
        
        std::cout << "All Pair tests passed!\n";
    } catch (const std::exception &e) {
        std::cerr << "Test failed: " << e.what() << '\n';
        return 1;
    }
    return 0;
}