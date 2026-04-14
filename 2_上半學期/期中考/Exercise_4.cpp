// main.cpp
#include <iostream>
#include "Dice.h"


int main() {
    srand(time(0));
    using namespace std;
    DiceGame game;
    game.playRound();
    game.displayResult();
}
