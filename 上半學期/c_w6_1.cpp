#include <iostream>

int p1 = 0;
int p2 = 0;

void autoscore(int p1, int p2)
{
	int roundScore[2] = { p1,p2 };
	printf("This round Score：P1:%d P2:%d\n", roundScore[0],roundScore[1]);

}

void staticscore(int p1, int p2)
{
	static int totalScore[2] = { 0,0 };
	totalScore[0] += p1;
	totalScore[1] += p2;
	printf("This round Total Score：P1:%d P2:%d\n", totalScore[0], totalScore[1]);
}



int main()
{
	srand((unsigned int)time(NULL));
    for (int i = 0; i < 3; i++) {
		int p1 = rand() % 10 + 1;
		int p2 = rand() % 10 + 1;
		printf("Rounbd %d Scores -> Pl:%d P2:%d\n",i+1, p1, p2);
		printf("[Round %d]\n",i+1);
		autoscore(p1, p2);
		staticscore(p1, p2);
		printf("\n");
    }

}