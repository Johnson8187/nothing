#include "Game.h"
#include <iostream>
#include <windows.h>  // Windows系統用
#include <conio.h>    // 用於_kbhit()和_getch()

using namespace std;

// 建構函式
Game::Game() {
    score = 0;
    dinosaurPos = 5;      // 恐龍固定在地面的X位置
    obstaclePos = 40;     // 障礙物初始位置
    isJumping = false;
    jumpHeight = 0;
}

// 清除螢幕（跨平台）
void clearScreen() {
    system("cls");  // Windows系統
    // 如果是Linux/Mac，改用 system("clear");
}

// 延遲函式
void delay(int milliseconds) {
    Sleep(milliseconds);  // Windows系統
}

// 渲染畫面
void Game::render() {
    clearScreen();

    cout << "===== 小恐龍遊戲 =====" << endl;
    cout << "分數: " << score << endl;
    cout << "按空白鍵跳躍 | 按 q 離開" << endl;
    cout << endl;

    // 跳躍行（對應你提供的 C 版本）
    if (jumpHeight > 0) {
        cout << "     O" << endl;
    }
    else {
        cout << endl;
    }

    // 地面行（50 寬）
    for (int i = 0; i < 50; i++) {
        if (i == dinosaurPos && jumpHeight == 0) {
            cout << "O";
        }
        else if (i == obstaclePos) {
            cout << "X";
        }
        else {
            cout << "_";
        }
    }
    cout << endl;
}

// 移動恐龍（跳躍邏輯）
void Game::moveDinosaur() {
    if (_kbhit()) {  // 檢查是否有按鍵
        char key = _getch();
        if (key == ' ' && jumpHeight == 0) {
            isJumping = true;
            jumpHeight = 3;
        }
        if (key == 'q' || key == 'Q') {
            exit(0);
        }
    }

    if (jumpHeight > 0) {
        jumpHeight--;
        isJumping = true;
    }
    else {
        isJumping = false;
    }
}

// 移動障礙物
void Game::moveObstacle() {
    obstaclePos--;
    if (obstaclePos < 0) {
        obstaclePos = 49;
    }
}

// 判斷是否撞到障礙物
bool Game::checkCollision() {
    // 對應你提供的 C 版本：treeX == 5 && jump == 0
    if (obstaclePos == dinosaurPos && jumpHeight == 0) {
        return true;
    }
    return false;
}

// 計算分數
void Game::calculateScore() {
    score++;  // 每一幀+1分
}

// 取得分數
int Game::getScore() {
    return score;
}

// 開始遊戲
void Game::startGame(Player& player) {
    cout << "\n玩家 " << player.getName() << " 開始遊戲！" << endl;
    cout << "按任意鍵開始..." << endl;
    _getch();

    score = 0;
    obstaclePos = 40;
    jumpHeight = 0;
    isJumping = false;

    while (true) {
        moveDinosaur();
        moveObstacle();
        calculateScore();
        render();

        if (checkCollision()) {
            render();
            cout << "\n遊戲結束！" << endl;
            cout << "你的分數: " << score << endl;

            player.updateHighScore(score);

            cout << "\n按任意鍵返回選單..." << endl;
            _getch();
            break;
        }

        delay(100);  // 控制遊戲速度
    }
}