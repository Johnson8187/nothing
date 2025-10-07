#include <iostream>

unsigned long long int factorial(int n) {
    unsigned long long int result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

int main()
{
    int n;
    printf("Enter n：");
    if (scanf_s("%d", &n) != 1) {
        printf("輸入錯誤。\n");
        return 1;
    }
    unsigned long long int n1 = factorial(n);
    printf("%llu\n", n1);

    return 0;
}
