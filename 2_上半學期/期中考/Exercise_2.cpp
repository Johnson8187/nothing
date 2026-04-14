#include <iostream>
using namespace std;

void swapvalue(int a,int b) {
    int temp;
    temp = a;
    a = b;
    b = temp;
    cout << "\nInside swapValue: x=" << a << " y=" << b;
}
void swappointer(int* a,int* b) {
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}


int main()
{
    int a = 10, b = 15;
    cout << "Before swap: a=" << a << " b=" << b;
    swapvalue(a, b);
    cout << "\nAfter swapValue: a=" << a << " b=" << b;
    swappointer(&a, &b);
    cout << "\nAfter swapPointer: a=" << a << " b=" << b;
}
