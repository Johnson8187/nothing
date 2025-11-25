#include <iostream>
#include <math.h>

int main()
{
	typedef struct
	{
		double x;
		double y;
	} Point;
	printf_s("How many Points do you want to enter? (<=10) :");
	int n;
	scanf_s("%d", &n);
	Point points[10];
	for(int i = 0; i < n; i++)
	{
		printf_s("Enter Point %d (x y): ", i );
		scanf_s("%lf %lf", &points[i].x, &points[i].y);
	}


    double minimum = sqrt(pow((points[1].x - points[0].x), 2) + pow((points[1].y - points[0].y), 2));
    int closet_x = points[1].x;
    int closet_y = points[1].y;
	for(int i = 0; i < n; i++)
	{
		double distance = sqrt(pow((points[i].x - points[0].x), 2) + pow((points[i].y - points[0].y), 2));
		if (i > 0) {
			if (distance < minimum) {
				minimum = distance;
				closet_x = points[i].x;
				closet_y = points[i].y;
			}
		}
	}
	printf_s("\nCloset point to P0 is (%d, %d), distance = %.3f\n", closet_x, closet_y,minimum);


	double maximum = sqrt(pow((points[1].x - points[0].x), 2) + pow((points[1].y - points[0].y), 2));
	int farthest_x = points[1].x;
	int farthest_y = points[1].y;
	for (int i = 0; i < n; i++)
	{
		double distance = sqrt(pow((points[i].x - points[0].x), 2) + pow((points[i].y - points[0].y), 2));
		if (distance > maximum) {
			maximum = distance;
			farthest_x = points[i].x;
			farthest_y = points[i].y;
		}
	}
	printf_s("Farthest point to P0 is (%d, %d), distance = %.3f", farthest_x, farthest_y, maximum);
	
	double total_x = 0;
	double total_y = 0;

	for (int i = 1; i < n; i++) {
		total_x = points[i].x + total_x;
		total_y = points[i].y + total_y;
	}
	double center_p[2] = { total_x / n,total_y / n };
	printf_s("\nCenter point =(%.3f, %.3f)", center_p[0], center_p[1]);
}


