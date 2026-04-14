#include "Player.h"
#include <iostream>

// 建構函式
Player::Player(string n, int num) {
    name = n;
    playerNumber = num;
    highScore = 0;
}

// 更新最高分
void Player::updateHighScore(int score) {
    if (score > highScore) {
        highScore = score;
    }
}

// 顯示玩家資訊
void Player::displayInfo() {
    cout << "玩家 " << playerNumber << ": " << name;
    cout << " | 最高分: " << highScore << endl;
}

// 取得玩家名稱
string Player::getName() {
    return name;
}

// 取得最高分
int Player::getHighScore() {
    return highScore;
}