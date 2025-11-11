#include <iostream>
#include <cstdio>
#include <cstdlib> 
#include <ctime>  

int x = 0;
int rollDice();
int p_mem = 0;
int player_score = 0;
int computer_score = 0;
int c_mem = 0;

int main()
{
	srand(time(NULL));
    printf("=== Lucky Battle：擲骰子對戰遊戲 ===\n");
	printf("Player's turn：");
    rollDice();
	p_mem = x;
	rollDice();
	player_score = x + p_mem;
	printf("You rolled %d and %d, your total score is %d\n", p_mem, x, player_score);
	rollDice();
	c_mem = x;
	rollDice();
	computer_score = x + c_mem;
	printf("Computer's turn：\nComputer rolled %d and %d, computer's total score is %d\n", c_mem, x, computer_score);
	if (player_score > computer_score) {
		printf("You win!\n");
	}
	else if (player_score < computer_score) {
		printf("Computer wins!\n");
	}
	else {
		printf("It's a tie!\n");
	}
	return 0;
}

int rollDice() {
    x = (rand() % 6) + 1;
    return x;
}
