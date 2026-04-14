
#include <iostream>

int main()
{
    int total = 0;
    int temp = 0;
    for (int i = 0; i<10; i++) {
        printf_s("Enter grade：");
        scanf_s("%d", &temp);
        total = total + temp;
        temp = 0;
    }
    printf_s("class average is %d", total / 10);
}

