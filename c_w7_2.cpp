#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define ROW 3
#define COL 3

void fillBoard(int a[ROW][COL]);
void printBoard(const int a[ROW][COL]);
int findFirst(const int a[ROW][COL], int key, int *r, int *c);

int main(void) {
    int board[ROW][COL];
    srand((unsigned)time(NULL));

    fillBoard(board);

    printf("Board (3x3):\n");
    printBoard(board);

    int key;
    printf("\nEnter a number to search (1-9): ");
    if (scanf("%d", &key) != 1) {
        fprintf(stderr, "Invalid input.\n");
        return 1;
    }

    int r, c;
    if (findFirst(board, key, &r, &c)) {
        printf("Found at (row=%d, col=%d)\n", r, c);  // 1-based
    } else {
        printf("Not found.\n");
    }

    return 0;
}

void fillBoard(int a[ROW][COL]) {
    int nums[ROW * COL];
    for (int i = 0; i < ROW * COL; ++i) nums[i] = i + 1; // 1..9


    for (int i = ROW * COL - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int tmp = nums[i];
        nums[i] = nums[j];
        nums[j] = tmp;
    }

    int k = 0;
    for (int r = 0; r < ROW; ++r) {
        for (int c = 0; c < COL; ++c) {
            a[r][c] = nums[k++];
        }
    }
}

void printBoard(const int a[ROW][COL]) {
    for (int r = 0; r < ROW; ++r) {
        for (int c = 0; c < COL; ++c) {
            printf("%d%c", a[r][c], (c == COL - 1) ? '\n' : ' ');
        }
    }
}

int findFirst(const int a[ROW][COL], int key, int *r, int *c) {
    for (int i = 0; i < ROW; ++i) {
        for (int j = 0; j < COL; ++j) {
            if (a[i][j] == key) {
                if (r) *r = i + 1; // 1-based
                if (c) *c = j + 1; // 1-based
                return 1;
            }
        }
    }
    return 0;
}