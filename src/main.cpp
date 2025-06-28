#include <compare>
#include <functional>
#include <iostream>

#include <coroutine>
#include <ranges>
#include <vector>
using namespace std;
  

class foo {
    public:

    int value;
    std::string email;
    float ffffff;

    friend std::weak_ordering operator<=>(const foo &lhs, const foo &rhs) {
        if (lhs.value > rhs.value) {
            return std::weak_ordering::greater;
        
        } else if (lhs.email < rhs.email) {
            return std::weak_ordering::greater;

        } else if (abs(lhs.ffffff - rhs.ffffff) < 1e6) {
            return std::weak_ordering::equivalent;
        }
        return std::weak_ordering::less;
    }

};


int main(){
   vector<int> vt;
   cout << vt.size();
    return 0;
}
