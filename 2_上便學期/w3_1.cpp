#include <iostream>
using namespace std;

// ========== Overloading 版本 ==========
int square(int x) {
    return x * x;
}

double square(double x) {
    return x * x;
}

// ========== Template 版本 ==========
template <typename T>
T squareTemplate(T x) {
    return x * x;
}

int main() {
    // 宣告變數
    int a = 5;
    double b = 3.2;
    
    // 呼叫 overloading 版本
    cout << "Overloading int: " << square(a) << endl;
    cout << "Overloading double: " << square(b) << endl;
    
    // 呼叫 template 版本
    cout << "Template int: " << squareTemplate(a) << endl;
    cout << "Template double: " << squareTemplate(b) << endl;
    
    return 0;
}