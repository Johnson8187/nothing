#pragma once

#include <string>

using namespace std;

class Player {
    string name;
    int highestScore;
public:
    Player(string n);
    void updateScore(int s);
    void info();
};
