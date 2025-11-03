#include <algorithm>
#include <cassert>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>

#include "vector.hpp"

using namespace CompactSTL;
struct Track {
    static int live;
    Track() { ++live; }
    Track(const Track &) { ++live; }
    ~Track() { --live; }
};
int Track::live = 0;
static void test_default_and_push_back() {
    vector<int> v;
    assert(v.size() == 0);
    size_t old_cap = v.capacity();
    for (int i = 0; i < 16; ++i) {
        v.push_back(i);
        assert(v.back() == i);
    }
    assert(v.size() == 16);
    assert(v.capacity() >= old_cap);
    // element access
    for (size_t i = 0; i < v.size(); ++i)
        assert(v[i] == static_cast<int>(i));
}

static void test_range_constructor_and_iterators() {
    std::vector<int> src = {10, 11, 12, 13, 14};
    auto b               = src.begin();
    auto e               = src.end();
    // range ctor in your vector expects iterator refs
    vector<int> v(b, e);
    assert(v.size() == src.size());
    // std::accumulate should work with your iterators
    int sum_std = std::accumulate(src.begin(), src.end(), 0);
    int sum_my  = std::accumulate(v.begin(), v.end(), 0);
    assert(sum_std == sum_my);
    // std::find
    auto it = std::find(v.begin(), v.end(), 12);
    assert(it != v.end() && *it == 12);
}

static void test_copy_and_move() {
    vector<int> a;

    for (int i = 0; i < 8; ++i)
        a.push_back(i * 2);

    vector<int> b = a; // copy ctor
    assert(b.size() == a.size());
    for (size_t i = 0; i < a.size(); ++i)
        assert(a[i] == b[i]);

    vector<int> c = std::move(a); // move ctor
    assert(c.size() == b.size());
    // a may be empty after move; at least c has elements
    for (size_t i = 0; i < c.size(); ++i)
        assert(c[i] == b[i]);

    // copy assignment via pass-by-value operator=
    vector<int> d;
    d = c;
    assert(d.size() == c.size());
    for (size_t i = 0; i < d.size(); ++i)
        assert(d[i] == c[i]);
}

static void test_emplace_and_pop() {
    vector<std::pair<int, int>> v;
    v.emplace_back(1, 2);
    v.emplace_back(3, 4);
    assert(v.size() == 2);
    assert(v[0].first == 1 && v[0].second == 2);
    // emplace at middle (insert before second element)
    auto it = v.begin();
    ++it;
    v.emplace(it, 10, 20);
    assert(v.size() == 3);
    // value inserted at index 1
    assert(v[1].first == 10 && v[1].second == 20);

    // pop_back
    v.pop_back();
    assert(v.size() == 2);
}

static void test_at_and_exception() {
    vector<int> v;
    v.push_back(5);
    v.push_back(6);
    assert(v.at(0) == 5);
    bool thrown = false;
    try {
        [[maybe_unused]] int x = v.at(10);
    } catch (const std::runtime_error &) { thrown = true; }
    assert(thrown);
}

static void test_clear_and_destruct() {

    {
        vector<Track> v;
        for (int i = 0; i < 5; ++i)
            v.emplace_back();
        assert(Track::live == 5);
        v.clear();
        assert(Track::live == 0);
    }
    assert(Track::live == 0);
}

int main() {
    try {
        test_default_and_push_back();
        test_range_constructor_and_iterators();
        test_copy_and_move();
        test_emplace_and_pop();
        test_at_and_exception();
        test_clear_and_destruct();
        std::cout << "All vector tests passed.\n";
    } catch (const std::exception &e) {
        std::cerr << "Test failure: " << e.what() << '\n';
        return 2;
    } catch (...) {
        std::cerr << "Unknown test failure\n";
        return 3;
    }
    return 0;
}