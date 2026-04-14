#include <iostream>
using namespace std;

void swapValues(int a, int b)
{
    int temp = a;
    a = b;
    b = temp;
	cout << "Inside swapValues a = " << a << ", b = " << b << std::endl;
}
void swapPointers(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main()
{
    int a = 10, b = 15;
	cout << "before swapValue a = " << a << ", b = " << b << std::endl;
    swapValues(a, b);
    cout << "after swapValue a = " << a << ", b = " << b << std::endl;
	swapPointers(&a, &b);
    cout<< "after swapPointer a = " << a << ", b = " << b << std::endl;
	return 0;
}
