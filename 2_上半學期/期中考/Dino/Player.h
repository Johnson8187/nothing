#pragma once
#include <string>
using namespace std;

class Player {
private:
    string name;        // 玩家名稱
    int playerNumber;   // 玩家編號
    int highScore;      // 最高分

public:
    // 建構函式
    Player(string n, int num);

    // 成員函式
    void updateHighScore(int score);  // 更新最高分
    void displayInfo();               // 顯示玩家資訊
    string getName();                 // 取得玩家名稱
    int getHighScore();               // 取得最高分
};

