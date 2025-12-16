#include <stdio.h>
#include <stdlib.h>
#include <conio.h>    // 用於 _kbhit() 和 _getch()
#include <windows.h>  // 用於 Sleep()
#include <time.h>     // 用於時間紀錄

int main() {
    // 變數宣告：W/H地圖大小, x/y蛇座標陣列, len長度, fx/fy食物座標, dir方向, score分數
    int W = 20, H = 15, x[100] = { 10 }, y[100] = { 7 }, len = 1;
    int fx, fy, dir = 0, score = 0, gameOver = 0, i, j, k;
    char name[50];

    srand((unsigned int)time(0)); // 隨機數種子 (修正 C4244)
    fx = rand() % W; fy = rand() % H; // 初始食物

    printf("請輸入名字: ");
    scanf_s("%s", name, (unsigned int)sizeof(name)); // (修正 C4996: scanf -> scanf_s)

    while (!gameOver) {
        system("cls"); // 清除螢幕 (為了動畫效果)

        // 1. 繪製地圖
        for (j = 0; j < H; j++) {
            for (i = 0; i < W; i++) {
                if (i == x[0] && j == y[0]) printf("O"); // 畫蛇頭
                else if (i == fx && j == fy) printf("$"); // 畫食物
                else {
                    int printBody = 0;
                    for (k = 1; k < len; k++)
                        if (x[k] == i && y[k] == j) printBody = 1;
                    if (printBody) printf("o"); // 畫蛇身
                    else printf("."); // 畫空地
                }
            }
            printf("\n");
        }
        printf("Score: %d\n", score);

        // 2. 接收鍵盤輸入 (方向鍵: 上72, 下80, 左75, 右77)
        if (_kbhit()) {
            switch (_getch()) {
            case 72: dir = 0; break; // 上
            case 80: dir = 1; break; // 下
            case 75: dir = 2; break; // 左
            case 77: dir = 3; break; // 右
            }
        }

        // 3. 移動身體 (從尾巴開始，繼承前一格的位置)
        for (k = len; k > 0; k--) { x[k] = x[k - 1]; y[k] = y[k - 1]; }

        // 4. 移動頭部
        if (dir == 0) y[0]--;      // 上
        else if (dir == 1) y[0]++; // 下
        else if (dir == 2) x[0]--; // 左
        else if (dir == 3) x[0]++; // 右

        // 5. 判斷吃食物
        if (x[0] == fx && y[0] == fy) {
            score++; len++;
            fx = rand() % W; fy = rand() % H; // 重新生成食物
        }

        // 6. 判斷死亡 (撞牆 或 撞身體)
        if (x[0] < 0 || x[0] >= W || y[0] < 0 || y[0] >= H) gameOver = 1;
        for (k = 1; k < len; k++)
            if (x[0] == x[k] && y[0] == y[k]) gameOver = 1;

        Sleep(100); // 控制遊戲速度 (毫秒)
    }
        
    // 7. 遊戲結束，寫入 CSV
    printf("Game Over!\n");
    
    FILE* fp = NULL;
    // (修正 C4996: fopen -> fopen_s)
    if (fopen_s(&fp, "scores.csv", "a") == 0 && fp != NULL) {
        time_t now = time(NULL);
        char timeStr[26];
        // (修正 C4996: ctime -> ctime_s)
        ctime_s(timeStr, sizeof(timeStr), &now);

        // 寫入格式：名字,分數,日期時間
        fprintf(fp, "%s,%d,%s", name, score, timeStr);
        fclose(fp);
    }

    system("pause");
    return 0;
}