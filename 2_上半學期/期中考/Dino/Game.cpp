#include "Game.h"
#include <iostream>
#include <conio.h>
#include <windows.h>

using namespace std;

void Game::play(Player& p) {
    int obsx = 20, score = 0, isjump = 0;
    while (1) {
        system("cls");
        cout << "score：" << score << endl;
        if (_kbhit() and _getch() == ' ')isjump = 4;
        if (isjump == 0 and obsx == 2)break;
        for (int i = 0; i < 25; i++) {
            if (i == 2) {
                if (isjump != 0)cout << "^";
                else cout << "D";
            }
            if (i == obsx)cout << "X";
            else cout << "_";
        }
        
        if (isjump != 0)isjump--;
        if (obsx < 0)obsx = 20;
        else obsx--;
        score++;
        Sleep(100);
    }

    cout << "\n\n 遊戲結束！\n";
    p.updateScore(score);                     
    system("pause");
}
