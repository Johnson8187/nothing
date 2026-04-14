// main.cpp
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Dice.h"


using namespace std;

int main() {
    srand(time(0));
    
    DiceGame game;
    game.playRound();
    game.displayResult();
    
    return 0;
}