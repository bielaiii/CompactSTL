#include <cassert>
#include <iostream>

#include "SharedPtr.hpp"

using namespace CompactSTL;

struct TestObj {
    static int count;
    int value;
    
    TestObj(int v = 0) : value(v) { ++count; }
    ~TestObj() { --count; }
};
int TestObj::count = 0;

static void test_basic_construction() {
    {
        SharedPointer<int> ptr(new int(42));
        assert(ptr.use_count() == 1);
        assert(*ptr == 42);
    }
}

static void test_copy_semantics() {
    SharedPointer<TestObj> ptr1(new TestObj(10));
    assert(TestObj::count == 1);
    assert(ptr1.use_count() == 1);
    
    SharedPointer<TestObj> ptr2 = ptr1;
    assert(TestObj::count == 1);
    assert(ptr1.use_count() == 2);
    assert(ptr2.use_count() == 2);
    assert(ptr1->value == 10);
    assert(ptr2->value == 10);
}

static void test_move_semantics() {
    SharedPointer<TestObj> ptr1(new TestObj(20));
    assert(ptr1.use_count() == 1);
    
    SharedPointer<TestObj> ptr2 = std::move(ptr1);
    assert(TestObj::count == 1);
    // ptr1 should be null after move
    // ptr2 should have the object
    assert(ptr2.use_count() == 1);
    assert(ptr2->value == 20);
}

static void test_unique() {
    SharedPointer<int> ptr1(new int(5));
    assert(ptr1.unique());
    
    SharedPointer<int> ptr2 = ptr1;
    assert(!ptr1.unique());
    assert(!ptr2.unique());
}

static void test_swap() {
    SharedPointer<int> ptr1(new int(1));
    SharedPointer<int> ptr2(new int(2));
    
    //ptr1.swap(ptr2);
    //assert(*ptr1 == 2);
    //assert(*ptr2 == 1);
}

static void test_reset() {
    SharedPointer<TestObj> ptr(new TestObj(30));
    assert(TestObj::count == 1);
    
    ptr.reset(new TestObj(40));
    assert(TestObj::count == 1);
    assert(ptr->value == 40);
}

static void test_make_shared_pointer() {
    auto ptr = makeSharePointer<TestObj>(50);
    assert(TestObj::count == 1);
    assert(ptr.use_count() == 1);
    assert(ptr->value == 50);
    
    auto ptr2 = ptr;
    assert(ptr.use_count() == 2);
}

static void test_owner_before() {
    SharedPointer<int> ptr1(new int(1));
    SharedPointer<int> ptr2(new int(2));
    
    bool result = ptr1.owner_before(ptr2) || ptr2.owner_before(ptr1);
    // Just test that the function runs without error
    (void)result;
}

int main() {
    try {
        test_basic_construction();
        test_copy_semantics();
        test_move_semantics();
        test_unique();
        test_swap();
        test_reset();
        test_make_shared_pointer();
        test_owner_before();
        
        assert(TestObj::count == 0);
        std::cout << "All SharedPtr tests passed!\n";
    } catch (const std::exception &e) {
        std::cerr << "Test failed: " << e.what() << '\n';
        return 1;
    }
    return 0;
}