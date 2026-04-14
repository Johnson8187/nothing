#include <iostream>
#include <stdio.h>

int num;

int main(void) {
    printf("請輸入學號：\n");
    scanf("%d", &num);
    while(num == 0) {
        printf("學號輸入錯誤，請重新輸（請輸入整數）\n");
        scanf("%d", &num);
    }
    printf("%d\n", num);
}