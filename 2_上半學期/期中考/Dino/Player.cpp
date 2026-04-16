#include "Player.h"
#include <iostream>

using namespace std;

Player::Player(string n) {
    name = n;
    highestScore = 0;
}

void Player::updateScore(int s) {
    if (s > highestScore) highestScore = s;
}

void Player::info() {
    cout << name << " (最高分: " << highestScore << ")\n";
}