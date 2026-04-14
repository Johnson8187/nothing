#include <iostream>
#include <stdexcept>
#include "Player.h"
#include "Game.h"

using namespace std;

int main() {
    // 建立3位玩家
    Player player1("Player 1", 1);
    Player player2("Player 2", 2);
    Player player3("Player 3", 3);

    Player* currentPlayer = nullptr;
    int choice;

    cout << "===== 小恐龍遊戲 =====" << endl;

    while (true) {
        try {
            cout << "\n請選擇玩家帳號：" << endl;
            cout << "1. Player 1" << endl;
            cout << "2. Player 2" << endl;
            cout << "3. Player 3" << endl;
            cout << "4. 顯示所有玩家資訊" << endl;
            cout << "5. 離開遊戲" << endl;
            cout << "請輸入選項 (1-5): ";
            cin >> choice;

            // 檢查輸入是否有效
            if (choice < 1 || choice > 5) {
                throw invalid_argument("無效的輸入！請輸入 1-5。");
            }

            if (choice == 4) {
                // 顯示所有玩家資訊
                cout << "\n===== 玩家資訊 =====" << endl;
                player1.displayInfo();
                player2.displayInfo();
                player3.displayInfo();
                continue;
            }

            if (choice == 5) {
                cout << "\n感謝遊玩！再見！" << endl;
                break;
            }

            // 選擇玩家
            switch (choice) {
            case 1:
                currentPlayer = &player1;
                break;
            case 2:
                currentPlayer = &player2;
                break;
            case 3:
                currentPlayer = &player3;
                break;
            }

            // 開始遊戲
            Game game;
            game.startGame(*currentPlayer);

        }
        catch (const invalid_argument& e) {
            cout << "\n錯誤: " << e.what() << endl;
            cout << "請重新輸入！\n" << endl;
            cin.clear();
            cin.ignore(10000, '\n');
        }
    }

    return 0;
}