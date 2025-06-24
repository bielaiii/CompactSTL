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
    foo f1{.value = 1, .email = "asdas@gmail.com",.ffffff= 121.4324};
    foo f2{.value = 1, .email = "asdas111@gmail.com", .ffffff = 121.4324};
    cout << "f1 <= f2: " << (f1 <= f2) << "\n";
    cout << "f2 < f1: " << (f2 < f1) << "\n";
    cout << "v1 > v2: " << (f1 > f2) << "\n";
    cout << "v2 >= v1: " << (f2 >= f1) << "\n";
    //cout << "v2 <=> v1: " << (f2 == f1) << "\n";
    return 0;
}
