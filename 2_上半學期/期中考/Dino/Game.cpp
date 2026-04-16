#include "Game.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

void Game::play(Player& p) {
    int obsX = 20;
    int score = 0;
    int jump = 0;

    // 清空鍵盤緩衝區
    while (_kbhit()) _getch();

    while (true) {
        // 偵測空格鍵跳躍
        if (_kbhit() && _getch() == ' ' && jump == 0) {
            jump = 4;
        }

        // 碰撞檢測：仙人掌在位置 2 且未跳起 → 遊戲結束
        if (obsX == 2 && jump == 0) break;

        system("cls");
        cout << "分數: " << score << "\n\n";

        // 繪製空中層（第 2 行）
        for (int i = 0; i < 25; i++) {
            if (i == 2 && jump > 0) cout << "D";   // 恐龍在空中
            else cout << " ";                      // 空白
        }
        cout << "\n";

        // 繪製地面層（第 3 行）
        for (int i = 0; i < 25; i++) {
            if (i == 2 && jump == 0) cout << "D";  // 恐龍在地面
            else if (i == obsX) cout << "X";       // 仙人掌
            else cout << "_";                      // 地板
        }

        // 更新狀態
        if (jump > 0) jump--;                     // 扣除滯空時間
        obsX = (obsX <= 0) ? 20 : obsX - 1;       // 仙人掌左移
        score++;
        Sleep(80);                                // 延遲控制速度
    }

    cout << "\n\n 遊戲結束！\n";
    p.updateScore(score);                         // 更新玩家最高分
    system("pause");
}