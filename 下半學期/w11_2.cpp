#include <iostream>


double avgTemp(double* temp) {
    double sum = 0;
    for (int i = 0; i < 7; ++i) sum += temp[i];
    return sum / 7;
}
double maxTemp(double* temp) {
    double max = temp[0];
    for (int i = 1; i < 7; ++i)
        if (temp[i] > max) max = temp[i];
    return max;
}
double countHotDays(double* temp) {
    int count = 0;
    for (int i = 0; i < 7; ++i)
        if (temp[i] >= 25.0) ++count;
    return count;
}

int main()
{
    double temp[7] = {22.5, 24.3, 23.0, 25.2, 24.8, 23.7, 26.1};
    double(*analysis[3])(double*) = { avgTemp, maxTemp, countHotDays };
    printf_s("Slect analysis (0=Avg,1=Max,2=Hot Days)：");
    int choice;
    scanf_s("%d", &choice);
    while (choice >= 0 && choice < 3) {
        double result = analysis[choice](temp);
        printf_s("Result: %.2f\n", result);
        printf_s("Slect analysis (0=Avg,1=Max,2=Hot Days)：");
        scanf_s("%d", &choice);
    }
        printf_s("Exit.\n");
}
