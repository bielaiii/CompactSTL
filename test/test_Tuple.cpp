#include <cassert>
#include <iostream>
#include <string>
#include <tuple>

#include "Tuple.hpp"

using namespace CompactSTL;
static void test_single_element_tuple() {
    Tuple<int> t(42);
    assert(t.get() == 42);
    assert(t.GetElement<0>() == 42);
}

static void test_multiple_element_tuple() {
    Tuple<int, double, std::string> t(10, 3.14, "hello");
    decltype(t.baseTuple.t) aa = 10;
    assert(Get<0>(t) == 10);
    assert(Get<1>(t) == 3.14);
    assert(Get<2>(t) == "hello");
}

static void test_two_element_tuple() {
    Tuple<std::string, int> t("test", 99);
    assert(Get<0>(t) == "test");
    assert(Get<1>(t) == 99);
}

static void test_nested_access() {
    Tuple<int, float, double> t(1, 2.5f, 3.14);
    assert(Get<0>(t) == 1);
    assert(Get<1>(t) == 2.5f);
    assert(Get<2>(t) == 3.14);
}

static void test_const_access() {
    const Tuple<int, std::string> t(42, "const");
    assert(CompactSTL::Get<0>(t) == 42);
    assert(CompactSTL::Get<1>(t) == "const");
}

static void test_reference_semantics() {
    Tuple<int, int> t(1, 2);

    auto &ref0 = Get<0>(t);
    auto &ref1 = Get<1>(t);

    // ref0 = 10;
    ref1 = 20;

    // assert(Get<0>(t) == 10);
    assert(Get<1>(t) == 20);
}
// todo
int main() {
    try {
        test_single_element_tuple();
        test_multiple_element_tuple();
        test_two_element_tuple();
        test_nested_access();
        test_const_access();
        test_reference_semantics();

        std::cout << "All Tuple tests passed!\n";
    } catch (const std::exception &e) {
        std::cerr << "Test failed: " << e.what() << '\n';
        return 1;
    }
  
    return 0;
}