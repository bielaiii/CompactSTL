#include "someFunctionality.hpp"
#include <iostream>
using namespace std;

int main() {
    using namespace CompactSTL;
    /* cout << CompactSTL::IsPtr<int>::value << endl;
    cout << CompactSTL::IsPtr<int *>::value << endl;
    cout << CompactSTL::IsPtr<int &>::value << endl;
 */
    using TT = int *;
    cout << !is_same_v<int *, CompactSTL::RemoveReference<int *>::mytype> << endl;
    cout << is_same_v<int *, CompactSTL::RemoveReference<int *>::mytype> << endl;
    cout << is_same_v<int, CompactSTL::RemoveReference<int &>::mytype> << endl;
    cout << is_same_v<int, CompactSTL::RemoveReference<int &&>::mytype> << endl;
    cout << is_same_v<int, CompactSTL::RemoveReference<int >::mytype> << endl;
    return 0;
}