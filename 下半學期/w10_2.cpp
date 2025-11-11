#include <stdio.h>


void swapInt(int *xPtr, int *yPtr) {
    int temp = *xPtr;
    *xPtr = *yPtr;
    *yPtr = temp;
}


void swapByValue(int x, int y) {
    int temp = x;
    x = y;
    y = temp;
    printf("Inside function: x = %d, y = %d\n", x, y);
}

int main(void) {
    int a, b;

    
    printf("Enter two integers: ");
    if (scanf_s("%d %d", &a, &b) != 2) return 1;
    printf("Before swap: a = %d, b = %d\n", a, b);
    swapInt(&a, &b);
    printf("After swap:  a = %d, b = %d\n\n", a, b);

    
    printf("Enter two integers: ");
    if (scanf_s("%d %d", &a, &b) != 2) return 1;
    printf("Before swap: a = %d, b = %d\n", a, b);
    swapByValue(a, b);
    printf("After swap:  a = %d, b = %d (no change)\n", a, b);

    return 0;
}