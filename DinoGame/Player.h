#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
    std::string name;
    int highestScore;
public:
    Player(std::string n);
    void updateScore(int s);
    void info();
};

#endif
