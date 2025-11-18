#include <cassert>
#include <iostream>
#include <memory>

#include "UniquePtr.hpp"

using namespace CompactSTL;

struct TestObject {
    static int instances;
    int value;

    TestObject(int v = 0) : value(v) { ++instances; }
    ~TestObject() { --instances; }
};
int TestObject::instances = 0;

static void test_basic_construction() {
    {
        UniquePtr<int> ptr(new int(42));
        assert(*ptr == 42);
        assert(ptr.get() != nullptr);
    }
    // ptr should be deleted here
}

static void test_move_semantics() {
    UniquePtr<TestObject> ptr1(new TestObject(10));
    assert(TestObject::instances == 1);
    assert(ptr1->value == 10);

    UniquePtr<TestObject> ptr2(std::move(ptr1));
    assert(TestObject::instances == 1);
    assert(ptr2->value == 10);
    // ptr1 should be null
    assert(ptr1.get() == nullptr);
}

static void test_reset() {
    UniquePtr<int> ptr(new int(5));
    assert(*ptr == 5);
    
    ptr.reset(new int(10));
    assert(*ptr == 10);
    
    ptr.reset();
    assert(ptr.get() == nullptr);
}

static void test_release() {
    int *raw = new int(42);
    UniquePtr<int> ptr(raw);
    int *released = ptr.release();
    
    assert(released == raw);
    assert(*released == 42);
    assert(ptr.get() == nullptr);
    
    delete released;
}

static void test_swap() {
    UniquePtr<int> ptr1(new int(1));
    UniquePtr<int> ptr2(new int(2));
    
    ptr1.swap(ptr2);
    assert(*ptr1 == 2);
    assert(*ptr2 == 1);
}

static void test_arrow_operator() {
    struct Data {
        int x = 0;
        int y = 0;
    };
    
    UniquePtr<Data> ptr(new Data{10, 20});
    assert(ptr->x == 10);
    assert(ptr->y == 20);
}

static void test_make_unique() {
    auto ptr = MakeUnique<TestObject>(99);
    assert(TestObject::instances == 1);
    assert(ptr->value == 99);
}

int main() {
    try {
        test_basic_construction();
        test_move_semantics();
        test_reset();
        test_release();
        test_swap();
        test_arrow_operator();
        test_make_unique();
        
        assert(TestObject::instances == 0);
        std::cout << "All UniquePtr tests passed!\n";
    } catch (const std::exception &e) {
        std::cerr << "Test failed: " << e.what() << '\n';
        return 1;
    }
    return 0;
}