#include <iostream>
#include <cstdio>

void toggleCase(char str[])
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (str[i] >= 'a' && str[i] <= 'z')
        {
            str[i] = str[i] - 32;
        }
        else if (str[i] >= 'A' && str[i] <= 'Z')
        {
            str[i] = str[i] + 32;
        }
        i++;
    }
    printf("Toggled：%s\n", str);
}

void reverse(char str[])
{
    int len = 0;
    while (str[len] != '\0') len++; 

    for (int i = 0; i < len / 2; ++i)
    {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
    printf("Reserved：%s\n", str);
    toggleCase(str);
}

int main()
{
    char str[100];
    printf("Enter a string：");
    scanf_s("%99s", str, (unsigned)sizeof(str));
    reverse(str);
}