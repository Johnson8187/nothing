#define _CRT_SECURE_NO_WARNINGS // 忽略安全警告，可使用標準 scanf/fopen
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>
int main() {
    int W = 20, H = 15, x[100] = { 10 }, y[100] = { 7 }, len = 1, score = 0;
    int fx, fy, dir = 0, gameOver = 0, i, j, k;
    int dx[] = { 0, 0, -1, 1 }, dy[] = { -1, 1, 0, 0 }; // 對應 上, 下, 左, 右 的座標變化
    char name[50];
    time_t now;
    srand((unsigned)time(0));
    fx = rand() % W; fy = rand() % H;
    printf("請輸入名字: ");
    scanf("%s", name);
    while (!gameOver) {
        system("cls");
        for (j = 0; j < H; j++) {
            for (i = 0; i < W; i++) {
                int body = 0;
                for (k = 1; k < len; k++) if (x[k] == i && y[k] == j) body = 1;
                printf((i == x[0] && j == y[0]) ? "O" : (i == fx && j == fy) ? "$" : body ? "o" : ".");
            }
            printf("\n");
        }
        printf("Score: %d\n", score);
        if (_kbhit()) {
            switch (_getch()) {
            case 72: dir = 0; break; // 上
            case 80: dir = 1; break; // 下
            case 75: dir = 2; break; // 左
            case 77: dir = 3; break; // 右
            }
        }
        for (k = len; k > 0; k--) { x[k] = x[k - 1]; y[k] = y[k - 1]; }
        x[0] += dx[dir];
        y[0] += dy[dir];
        if (x[0] == fx && y[0] == fy) {
            score++; len++;
            fx = rand() % W; fy = rand() % H;
        }
        if (x[0] < 0 || x[0] >= W || y[0] < 0 || y[0] >= H) gameOver = 1;
        for (k = 1; k < len; k++) if (x[0] == x[k] && y[0] == y[k]) gameOver = 1;

        Sleep(100);
    }
    printf("Game Over!\n");
    FILE* fp = fopen("scores.csv", "a");
    if (fp) {
        time(&now);
        fprintf(fp, "%s,%d,%.24s\n", name, score, ctime(&now));
        fclose(fp);
    }
    system("pause");
    return 0;
}