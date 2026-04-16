#include <iostream>
#include <stdexcept>
#include "Player.h"
#include "Game.h"

using namespace std;

int main() {
    Player p[3] = { Player("Player 1"), Player("Player 2"), Player("Player 3") };

    while (true) {
        system("cls");
        for (int i = 0; i < 3; i++) {
            cout << i + 1 << ". ";
            p[i].info();
        }
        cout << "4. 離開\n選擇: ";
        int c;
        try {
            if (!(cin >> c) || c < 1 || c > 4) {
                cin.clear();
                cin.ignore(100, '\n');
                throw invalid_argument("只能輸入 1~4 哦！");
            }
            if (c == 4) break;
            Game().play(p[c - 1]);
        }
        catch (const invalid_argument& e) {
            cout << "錯誤: " << e.what() << "\n";
            system("pause");
        }
    }
    cout << "系統已關閉！\n";
    return 0;
}
