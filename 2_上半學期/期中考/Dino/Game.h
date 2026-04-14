#pragma once
#include "Player.h"

class Game {
private:
    int score;              // 遊戲分數
    int dinosaurPos;        // 恐龍位置（行）
    int obstaclePos;        // 障礙物位置（列）
    bool isJumping;         // 是否正在跳躍
    int jumpHeight;         // 跳躍高度

public:
    // 建構函式
    Game();

    // 成員函式
    void startGame(Player& player);     // 開始遊戲
    bool checkCollision();              // 判斷是否撞到障礙物
    void calculateScore();              // 計算分數
    void render();                      // 渲染畫面
    void moveDinosaur();                // 移動恐龍
    void moveObstacle();                // 移動障礙物
    int getScore();                     // 取得分數
};
