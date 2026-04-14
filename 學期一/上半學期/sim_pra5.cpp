#include <iostream>
#include <cstdio>
#include <cstring>

#include <stdio.h>

int main()
{
    char input[20];
    printf("Enter a string:");
    scanf_s("%19s", input, (unsigned)_countof(input));

    int i = 0;
    while (input[i] != '\0') {
        if (i > 0) {
            printf(" ");
        }
        printf("%c", input[i]);
        i++;
    }

    return 0;
}
