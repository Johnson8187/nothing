
#include <iostream>

int main()
{
    int g1 = 0;
    int g2 = 0;
    printf_s("Enter two integers:");
    scanf_s("%d%d", &g1,&g2);
    if (g1 == g2) {
        printf_s("%d is equal to %d\n", g1, g2);
    }
    else
    {
        printf_s("%d is not equal to %d\n", g1, g2);
        if (g1 < g2)
        {
            printf_s("%d is less than %d\n", g1, g2);
        }
        else {
            printf_s("%d is more than %d\n", g1, g2);
        }

        if (g1 <= g2) {
            printf_s("%d is less than or equal to %d\n", g1, g2);
        }
        else {
            printf_s("%d is more than or equal to %d\n", g1, g2);
        }
    }
}

