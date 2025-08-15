#include "function.hpp"
#include <functional>
#include <print>


int main() {
    int buf[1024];
    auto caller = [buf](int a) -> int { return 1 + a; };
    std::println("{}", sizeof(caller));
    std::function<int(int)> fgt = caller;
    fgt(10);
    std::vector<int> stdvt;
    stdvt.push_back(1);
    stdvt.push_back(1);
    stdvt.push_back(1);
    stdvt.push_back(1);
    stdvt.clear();
    return 0;
}