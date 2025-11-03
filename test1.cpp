#include <cstddef>
#include <iostream>
#include <iterator>
#include <print>
#include <type_traits>
#include <vector>
#include"vector.hpp"


#include <atomic>
#include <cassert>
#include <string>
#include <thread>
 using namespace CompactSTL;

 
int main()
{
    std::vector<int> stdvec{1,2,3,4,5};
    stdvec.capacity();
    
    stdvec.begin();
    vector<int> vec;
    vec.push_back(1);
    vec.push_back(2);
    vec.push_back(3);
    vec.push_back(4);
    auto it = vec.begin();
    it++;
    ++it;
    println("{}", *it);
    return 0;
}