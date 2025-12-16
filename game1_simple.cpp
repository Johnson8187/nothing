#include <stdio.h>
#include <stdlib.h>
#include <conio.h>    
#include <windows.h> 
int main() {
    int treeX = 40, jump = 0, score = 0;
    while (1) { 
        system("cls"); 
        printf("\n"); 
        if (jump > 0) printf("     O\n"); 
        else printf("\n");
        for (int i = 0; i < 50; i++) {
            if (i == 5 && jump == 0) printf("O");      
            else if (i == treeX)     printf("X");      
            else                     printf("_");    
        }
        printf("\nScore: %d", score++);
        if (_kbhit()) {
            if (_getch() == ' ' && jump == 0) jump = 3;
        }
        if (jump > 0) jump--; 
        treeX--;              
        if (treeX < 0) treeX = 49;
        if (treeX == 5 && jump == 0) break;
        Sleep(100);
    }
    printf("\nGAME OVER!\n");
    system("pause");
    return 0;
}