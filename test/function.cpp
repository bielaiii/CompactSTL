#include "function.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

// A global counter to track constructor/destructor calls for memory management
// tests
static int object_counter = 0;

// A small object that is not trivially destructible
struct SmallObject {
    int id;
    SmallObject(int i = 0) : id(i) {
        std::cout << "SmallObject(" << id << ") constructed." << std::endl;
        object_counter++;
    }
    ~SmallObject() {
        std::cout << "SmallObject(" << id << ") destroyed." << std::endl;
        object_counter--;
    }
    SmallObject(const SmallObject &other) : id(other.id) {
        std::cout << "SmallObject(" << id << ") copy constructed." << std::endl;
        object_counter++;
    }
    SmallObject &operator=(const SmallObject &other) {
        if (this != &other) {
            id = other.id;
        }
        return *this;
    }
    void operator()(int a) {
        std::cout << "SmallObject(" << id << ") call: " << a << std::endl;
    }
    int operator()(int a, int b) const { return id + a + b; }
};

// A larger object to force heap allocation
struct LargeObject {
    int id;
    std::vector<int> data;
    LargeObject(int i = 0) : id(i), data(20, i) {
        std::cout << "LargeObject(" << id << ") constructed." << std::endl;
        object_counter++;
    }
    ~LargeObject() {
        std::cout << "LargeObject(" << id << ") destroyed." << std::endl;
        object_counter--;
    }
    LargeObject(const LargeObject &other) : id(other.id), data(other.data) {
        std::cout << "LargeObject(" << id << ") copy constructed." << std::endl;
        object_counter++;
    }
    void operator()() const {
        std::cout << "LargeObject(" << id << ") called." << std::endl;
    }
    int operator()(int val) const { return id + val; }
};

// A trivially destructible object to test memcpy path
struct TrivialObject {
    int value;
    int get_move{};
    TrivialObject(int v = 0) : value(v) {
        std::cout << "TrivialObject(" << value << ") constructed." << std::endl;
        object_counter++;
    }
    TrivialObject(TrivialObject &&other) :value(other.value) {
        std::cout << "Move TrivialObject(" << value << ") constructed."
                  << std::endl;
        // object_counter++;

        get_move = 1;
    }
    TrivialObject(TrivialObject &other) {
        std::cout << "Copy TrivialObject(" << value << ") constructed."
                  << std::endl;
        object_counter++;
    }
    ~TrivialObject() {
        std::cout << "TrivialObject(" << value << ") destroyed." << std::endl;
        if (get_move) {
            object_counter--;
        }
    }
    int operator()() const { return value; }
};

// A free function
void greet() { std::cout << "Hello from a free function!" << std::endl; }

// A free function with arguments and a return value
int add(int a, int b) { return a + b; }

// Global variable for testing stateful lambdas
int global_counter = 0;

void test_section(const std::string &title) {
    std::cout << "\n--- " << title << " ---" << std::endl;
}

int main() {
    using namespace CompactSTL;
    // static_assert(std::is_trivially_move_constructible_v<TrivialObject>,"asdas"
    // );
    std::cout << "Testing CompactSTL::functional implementation...\n"
              << std::endl;
    std::cout << "Size of SmallObject: " << sizeof(SmallObject) << " bytes"
              << std::endl;
    std::cout << "Size of LargeObject: " << sizeof(LargeObject) << " bytes"
              << std::endl;
    std::cout << "Size of a pointer: " << sizeof(void *) << " bytes"
              << std::endl;
    std::cout << "4 pointers size: " << sizeof(void *) * 4 << " bytes"
              << std::endl;
    std::cout << std::boolalpha;
    std::cout << "SmallObject stores locally: "
              << (sizeof(SmallObject) < sizeof(void *) * 4 &&
                  alignof(SmallObject) <= 4)
              << std::endl;
    std::cout << "LargeObject stores locally: "
              << (sizeof(LargeObject) < sizeof(void *) * 4 &&
                  alignof(LargeObject) <= 4)
              << std::endl;
    std::cout << std::noboolalpha;
    std::cout << "--------------------------------\n" << std::endl;

    // --- Test 1: Default Constructor ---
    test_section("Test 1: Default Constructor & operator bool");
    functional<void()> empty_func;
    assert(!empty_func);
    std::cout << "Empty functional bool operator works." << std::endl;

    // --- Test 2: Free Function ---
    test_section("Test 2: Free Function Callable");
    functional<void()> free_func(greet);
    assert(free_func);
    free_func();
    functional<int(int, int)> free_add(add);
    assert(free_add);
    int sum = free_add(10, 20);
    assert(sum == 30);
    std::cout << "Free function callable works. Result: " << sum << std::endl;

    // --- Test 3: Lambda Callables ---
    test_section("Test 3: Lambda Callables (stateless and stateful)");
    // Stateless lambda
    functional<void()> stateless_lambda(
        []() { std::cout << "Stateless lambda called." << std::endl; });
    stateless_lambda();
    // Stateful lambda
    int x = 5;
    functional<int(int)> stateful_lambda([&x](int y) { return x + y; });
    int result = stateful_lambda(5);
    assert(result == 10);
    std::cout << "Stateful lambda result: " << result << std::endl;
    // Another stateful lambda, capturing a global variable
    functional<void()> global_lambda([&]() {
        global_counter++;
        std::cout << "Global counter incremented to: " << global_counter
                  << std::endl;
    });
    global_lambda();
    assert(global_counter == 1);

    // --- Test 4: Functors (Small Object Optimization) ---
    test_section(
        "Test 4: Functors with Small Object Optimization (local storage)");
    int initial_object_count = object_counter;
    {
        functional<void(int)> small_func(SmallObject(1));
        assert(small_func);
        small_func(100);
        assert(object_counter ==
               initial_object_count + 1); // Should only have one instance
        functional<int(int, int)> small_func2(SmallObject(2));
        int res = small_func2(1, 2);
        assert(res == 5);
        std::cout << "SmallObject functor call works. Result: " << res
                  << std::endl;

        // Test TrivialObject (should use memcpy)
        functional<int()> trivial_func(TrivialObject(42));
        assert(object_counter == initial_object_count + 3);
        int val = trivial_func();
        assert(val == 42);
        std::cout << "TrivialObject functor call works. Result: " << val
                  << std::endl;
    }
    // After scope, SmallObject and TrivialObject destructors should have been
    // called
    assert(object_counter == initial_object_count);
    std::cout << "All small objects correctly destructed." << std::endl;

    // --- Test 5: Functors (Large Object, heap allocation) ---
    test_section("Test 5: Functors with Large Object (heap allocation)");
    initial_object_count = object_counter;
    {
        functional<void()> large_func(LargeObject(99));
        assert(large_func);
        large_func();
        assert(object_counter ==
               initial_object_count +
                   1); // Should have one heap-allocated instance
        functional<int(int)> large_func2(LargeObject(100));
        int res = large_func2(10);
        assert(res == 110);
        std::cout << "LargeObject functor call works. Result: " << res
                  << std::endl;
        assert(object_counter == initial_object_count + 2);
    }
    // After scope, both LargeObject destructors should have been called
    assert(object_counter == initial_object_count);
    std::cout << "All large objects correctly destructed." << std::endl;

    // --- Test 6: Copy and Move Semantics ---
    test_section("Test 6: Copy & Move Semantics");
    {
        functional<int(int, int)> original(add);
        assert(original);
        assert(original(1, 2) == 3);

        // Copy constructor test
        functional<int(int, int)> copy_of_original(original);
        assert(copy_of_original);
        assert(copy_of_original(3, 4) == 7);
        // Verify original is still valid and independent
        assert(original(10, 20) == 30);
        std::cout << "Copy constructor works." << std::endl;

        // Move constructor test
        functional<int(int, int)> moved_from_original(std::move(original));
        assert(moved_from_original);
        assert(!original); // Original should be invalid
        assert(moved_from_original(50, 60) == 110);
        // Calling moved-from object should do nothing
        int moved_from_call = 0;
        try {
            moved_from_call = original(1, 1);
        } catch (...) {
            std::cout << "Correctly caught exception on moved-from call."
                      << std::endl;
        }
        assert(moved_from_call == 0); // Should return default-init value
        std::cout << "Move constructor works." << std::endl;
    }

    // --- Test 7: swap() method ---
    test_section("Test 7: swap() method");
    functional<int()> func1([]() { return 1; });
    functional<int()> func2([]() { return 2; });
    assert(func1() == 1 && func2() == 2);
    std::cout << "func1 before swap: " << func1()
              << ", func2 before swap: " << func2() << std::endl;
    func1.swap(func2);
    assert(func1() == 2 && func2() == 1);
    std::cout << "func1 after swap: " << func1()
              << ", func2 after swap: " << func2() << std::endl;

    std::cout << "\nAll tests passed successfully!" << std::endl;
    assert(object_counter == 0);
    std::cout << "Final object count is 0. All objects correctly destructed."
              << std::endl;

    return 0;
}