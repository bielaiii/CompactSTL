#include "function.hpp"
#include <functional>
#include <print>

using std::print;
using std::println;
int add(int a, double b) { return a + b; }

struct fuc {
    int b = 100;
    fuc() : b(100) {};
    fuc(const fuc &other) { println(stderr, "copy ont"); }

    int adda(int a) const { return a * b; }
    int operator()(int a) { return a * 200; }
    ~fuc() {
        std::println(stderr, "call destructor");
        std::println(stderr, "eerrr");
    };
};

int ddddd(int a) { return a + 10; }

int main() {

    using namespace CompactSTL;
    auto tempf  = fuc();
    auto myfunc = CompactSTL::functional(tempf);
    /*     println("{}", template_select<decltype(tempf)>::value);
        // println("{}", std::is_pointer_v<decltype(tempf)>);
        println("{}", template_select<decltype(ddddd)>::value); */
    print(">{}<", myfunc(100));

    /* auto f       = fuc();
   auto stdfunc = std::function(f);
   println("{}", stdfunc(99)); */
    return 0;
}