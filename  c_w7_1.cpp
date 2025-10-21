#include <stdio.h>

#define STUDENTS 3
#define EXAMS 4

int minimum(const int scores[][EXAMS], int students, int exams);
int maximum(const int scores[][EXAMS], int students, int exams);
double average(const int scores[], int exams);
void printScores(const int scores[][EXAMS], int students, int exams);

int main(void) {
    int scores[STUDENTS][EXAMS];

    for (int i = 0; i < STUDENTS; ++i) {
        printf("Enter %d scores for student %d: ", EXAMS, i + 1);
        for (int j = 0; j < EXAMS; ++j) {
            if (scanf("%d", &scores[i][j]) != 1) {
                fprintf(stderr, "\nInvalid input. Please enter integers.\n");
                return 1;
            }
        }
    }

    puts("\nScores:");
    printScores(scores, STUDENTS, EXAMS);

    int minScore = minimum(scores, STUDENTS, EXAMS);
    int maxScore = maximum(scores, STUDENTS, EXAMS);

    printf("\nMinimum score in class: %d\n", minScore);
    printf("Maximum score in class: %d\n", maxScore);

    puts("\nAverages:");
    for (int i = 0; i < STUDENTS; ++i) {
        double avg = average(scores[i], EXAMS);
        printf("Student %d -> %.2f\n", i + 1, avg);
    }

    return 0;
}

int minimum(const int scores[][EXAMS], int students, int exams) {
    int min = scores[0][0];
    for (int i = 0; i < students; ++i) {
        for (int j = 0; j < exams; ++j) {
            if (scores[i][j] < min) {
                min = scores[i][j];
            }
        }
    }
    return min;
}

int maximum(const int scores[][EXAMS], int students, int exams) {
    int max = scores[0][0];
    for (int i = 0; i < students; ++i) {
        for (int j = 0; j < exams; ++j) {
            if (scores[i][j] > max) {
                max = scores[i][j];
            }
        }
    }
    return max;
}

double average(const int scores[], int exams) {
    int sum = 0;
    for (int j = 0; j < exams; ++j) {
        sum += scores[j];
    }
    return (double)sum / exams;
}

void printScores(const int scores[][EXAMS], int students, int exams) {
    for (int i = 0; i < students; ++i) {
        printf("Student %d: ", i + 1);
        for (int j = 0; j < exams; ++j) {
            printf("%4d", scores[i][j]);
        }
        putchar('\n');
    }
}