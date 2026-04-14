#include <iostream>
#define SIZE 5

int main()
{
    int data[SIZE] = { 2, 4, 6, 8, 10 };

    printf("Original array: ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");


    int* ptr = data;
    for (int i = 0; i < SIZE; i++) {
        *(ptr + i) *= 2;
    }

    printf("Array after doubling (pointer): ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", data[i]);
    }
    printf("\n");


    int data2[SIZE] = { 2, 4, 6, 8, 10 };
    for (int i = 0; i < SIZE; i++) {
        data2[i] *= 2;
    }

    printf("Array after doubling (index): ");
    for (int i = 0; i < SIZE; i++) {
        printf("%d ", data2[i]);
    }
    printf("\n");

    return 0;
}


