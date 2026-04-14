#include <iostream>
using namespace std;

class DiceGame {
private:
    int playerTotal;
    int computerTotal;
    
public:
    DiceGame() {
        playerTotal = 0;
        computerTotal = 0;
    }
    
    int rollDice() {
        int die1 = rand() % 6 + 1;
        int die2 = rand() % 6 + 1;
        cout << die1 << " + " << die2 << " = " << (die1 + die2) << endl;
        return die1 + die2;
    }
    
    void playRound() {
        cout << "Player's turn." << endl;
        playerTotal = rollDice();
        
        cout << "Computer's turn." << endl;
        computerTotal = rollDice();
        
        cout << endl;
    }
    
    void displayResult() {
        cout << "Player total: " << playerTotal << endl;
        cout << "Computer total: " << computerTotal << endl;
        
        if (playerTotal > computerTotal) {
            cout << "You win!" << endl;
        } else if (playerTotal < computerTotal) {
            cout << "You lose!" << endl;
        } else {
            cout << "It's a tie!" << endl;
        }
    }
};

