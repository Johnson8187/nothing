// main.cpp
#include <iostream>
#include "Dice.h"


int main() {
    srand(time(0));
    DiceGame game;
    game.playRound();
    game.displayResult();
}
