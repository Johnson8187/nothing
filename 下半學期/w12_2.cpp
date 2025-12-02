#include <stdio.h>
#include <math.h>

// 定義點結構體，使用 Point 標籤，這在 C 語言中是標準做法。
typedef struct Point
{
    double x;
    double y;
} Point;

// 計算兩點間距離的輔助函數
double calculate_distance(Point p1, Point p2) {
    // 距離公式：sqrt((x2-x1)^2 + (y2-y1)^2)
    // [Image of the Distance Formula in a 2D plane]
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

int main()
{
    // 使用常數定義數組大小，提高程式碼可讀性和維護性
    const int MAX_POINTS = 10;
    Point points[MAX_POINTS];
    
    printf("How many Points do you want to enter? (<=%d): ", MAX_POINTS);
    int n;
    
    // 使用標準 scanf 讀取輸入
    if (scanf("%d", &n) != 1 || n < 2 || n > MAX_POINTS) {
        printf("Invalid number of points. Exiting.\n");
        return 1;
    }

    // 讀取所有點的座標
    for (int i = 0; i < n; i++)
    {
        printf("Enter Point P%d (x y): ", i);
        // 使用標準 scanf 讀取 double 類型
        if (scanf("%lf %lf", &points[i].x, &points[i].y) != 2) {
            printf("Invalid coordinate input. Exiting.\n");
            return 1;
        }
    }

    // ----------------------------------------------------
    // 1. 尋找最接近和最遠點 (相對於 P0)
    // ----------------------------------------------------

    // 如果點數少於 2，則無法比較，但我們在前面已經檢查 n >= 2
    if (n < 2) return 0;

    // 將 P1 作為初始的 Closest/Farthest 點
    double minimum_distance = calculate_distance(points[0], points[1]);
    double maximum_distance = minimum_distance;
    
    // 儲存最接近/最遠點的座標 (使用 double 避免數據遺失)
    Point closest_point = points[1];
    Point farthest_point = points[1];

    // 從 P2 (i=2) 開始遍歷，因為 P1 (i=1) 已經作為初始值
    for (int i = 2; i < n; i++)
    {
        // 關鍵改進：只計算一次距離
        double current_distance = calculate_distance(points[0], points[i]);

        // 檢查是否是最接近點
        if (current_distance < minimum_distance) {
            minimum_distance = current_distance;
            closest_point = points[i];
        }
        
        // 檢查是否是最遠點
        if (current_distance > maximum_distance) {
            maximum_distance = current_distance;
            farthest_point = points[i];
        }
    }

    // 輸出結果 (使用 %.3f 格式化浮點數)
    printf("\nClosest point to P0 is (%.3f, %.3f), distance = %.3f\n", 
           closest_point.x, closest_point.y, minimum_distance);

    printf("Farthest point to P0 is (%.3f, %.3f), distance = %.3f\n", 
           farthest_point.x, farthest_point.y, maximum_distance);


    // ----------------------------------------------------
    // 2. 計算中心點 (所有 n 個點的平均座標)
    // ----------------------------------------------------
    double total_x = 0.0;
    double total_y = 0.0;

    // 關鍵改進：循環從 i = 0 開始，包含所有 n 個點
    for (int i = 0; i < n; i++) {
        total_x += points[i].x;
        total_y += points[i].y;
    }
    
    // 計算平均值
    double center_x = total_x / n;
    double center_y = total_y / n;

    printf("\nCenter point (Average of all %d points) = (%.3f, %.3f)\n", n, center_x, center_y);

    return 0;
}