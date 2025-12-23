#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <conio.h>  
#include <windows.h>
#include <time.h>    


// 定義座標點
typedef struct {
    int x;
    int y;
} Point;

// 定義玩家資訊
typedef struct {
    char name[20];  // 儲存玩家名字
    int score;      // 目前分數
} Player;

// 定義蛇的屬性
typedef struct {
    Point body[100]; // 蛇的身體，最多 100 節
    int length;      // 目前長度
    int dir;         // 方向 (1:上, 2:下, 3:左, 4:右)
} Snake;

// 定義食物
typedef struct {
    Point pos;      // 食物位置
    int type;       // 0: 普通食物
} Food;

// --- 全域變數 ---
int width = 40;     // 地圖寬度
int height = 20;    // 地圖高度
int gameOver = 0;   // 遊戲結束旗標


void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


// color代碼: 7=白, 10=綠, 12=紅, 14=黃, 11=藍
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}


void hideCursor() {
    CONSOLE_CURSOR_INFO cursorInfo;
    cursorInfo.dwSize = 1;
    cursorInfo.bVisible = FALSE; // 設為不可見
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

// --- 遊戲邏輯函式 ---

void Setup(Snake* snake, Food* food, Player* player) {
    gameOver = 0;

    // 初始化蛇
    snake->length = 1;
    snake->body[0].x = width / 2;
    snake->body[0].y = height / 2;
    snake->dir = 0; // 0 代表暫停不動

    // 初始化食物
    food->pos.x = rand() % (width - 2) + 1;
    food->pos.y = rand() % (height - 2) + 1;
    food->type = 0;

    // 初始化玩家分數
    player->score = 0;

    // --- [2. 玩家輸入] 符合您的 fgets 要求 ---
    printf("=== SNAKE GAME ===\n");
    printf("請輸入玩家姓名 (Enter name): ");

    // 使用 fgets 讀取包含空格的名字
    fgets(player->name, 20, stdin);

    // 手動移除換行符號
    for (int i = 0; i < 20; i++) {
        if (player->name[i] == '\n') {
            player->name[i] = '\0';
            break;
        }
    }

    system("cls"); // 清空螢幕準備開始
}

void Draw(Snake* snake, Food* food, Player* player) {
    gotoxy(0, 0); // 每次畫圖都回到左上角，而不是清空螢幕，這樣才不會閃

    // 顯示資訊欄
    setColor(15); // 白色
    printf("Player: ");
    setColor(11); // 亮藍色
    printf("%-10s", player->name); // 顯示名字
    setColor(15);
    printf(" Score: ");
    setColor(14); // 黃色
    printf("%d\n", player->score);
    setColor(7);  // 恢復白色

    // 繪製地圖
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {

            // 畫牆壁
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                setColor(8); // 灰色牆壁
                printf("#");
            }
            // 畫蛇頭
            else if (x == snake->body[0].x && y == snake->body[0].y) {
                setColor(10); // 綠色蛇頭
                printf("O");
            }
            // 畫食物
            else if (x == food->pos.x && y == food->pos.y) {
                if (food->type == 1) {
                    setColor(14); // 黃金蘋果 (黃色)
                    printf("$");
                }
                else {
                    setColor(12); // 普通蘋果 (紅色)
                    printf("@");
                }
            }
            else {
                // 檢查是不是蛇身
                int isBody = 0;
                for (int k = 1; k < snake->length; k++) {
                    if (snake->body[k].x == x && snake->body[k].y == y) {
                        setColor(2); // 深綠色蛇身
                        printf("o");
                        isBody = 1;
                        break;
                    }
                }
                if (!isBody) printf(" "); // 空地
            }
        }
        printf("\n");
    }
}

void Input(Snake* snake) {
    // _kbhit() 檢查是否有按鍵按下，不會卡住程式
    if (_kbhit()) {
        // _getch() 讀取按鍵但不顯示在螢幕上
        switch (_getch()) {
        case 'a': if (snake->dir != 4) snake->dir = 3; break; // 左
        case 'd': if (snake->dir != 3) snake->dir = 4; break; // 右
        case 'w': if (snake->dir != 2) snake->dir = 1; break; // 上
        case 's': if (snake->dir != 1) snake->dir = 2; break; // 下
        case 'x': gameOver = 1; break; // 離開
        }
    }
}

void Logic(Snake* snake, Food* food, Player* player) {
    if (snake->dir == 0) return; // 遊戲剛開始尚未移動

    // 1. 移動蛇身 (從尾巴開始，每一節移到前一節的位置)
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }

    // 2. 移動蛇頭
    switch (snake->dir) {
    case 1: snake->body[0].y--; break; // 上
    case 2: snake->body[0].y++; break; // 下
    case 3: snake->body[0].x--; break; // 左
    case 4: snake->body[0].x++; break; // 右
    }

    // 3. 檢查是否撞牆 (創意功能：穿牆模式)
    // 這裡我們設定為「撞牆死亡」，若要改穿牆可以修改這裡
    if (snake->body[0].x <= 0 || snake->body[0].x >= width - 1 ||
        snake->body[0].y <= 0 || snake->body[0].y >= height - 1) {
        gameOver = 1;
    }

    // 4. 檢查是否撞到自己
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x &&
            snake->body[0].y == snake->body[i].y) {
            gameOver = 1;
        }
    }

    // 5. 檢查是否吃到食物
    if (snake->body[0].x == food->pos.x && snake->body[0].y == food->pos.y) {
        // 增加分數
        player->score += (food->type == 1) ? 50 : 10;

        // 增加長度
        snake->length++;

        // 隨機生成新食物
        food->pos.x = rand() % (width - 2) + 1;
        food->pos.y = rand() % (height - 2) + 1;

        // 創意功能：10% 機率生成黃金蘋果
        food->type = (rand() % 10 == 0) ? 1 : 0;
    }
}

int main() {
    // 設定亂數種子
    srand((unsigned int)time(NULL));

    // 隱藏游標，讓畫面更像遊戲
    hideCursor();

    // 宣告變數 (使用 typedef 定義的結構)
    Snake snake;
    Food food;
    Player player;

    // 遊戲初始化
    Setup(&snake, &food, &player);

    // 遊戲主迴圈
    while (!gameOver) {
        Draw(&snake, &food, &player);
        Input(&snake);
        Logic(&snake, &food, &player);

        int speed = 200 - (player.score / 50 * 5);
        if (speed < 50) speed = 50; // 最快速度上限也稍微放寬

        Sleep(speed);
    }

    // 遊戲結束畫面
    system("cls");
    setColor(12); // 紅色
    printf("\n\n");
    printf("   GAME OVER !!\n");
    setColor(7); // 白色
    printf("   Player: %s\n", player.name);
    printf("   Final Score: %d\n", player.score);
    printf("\n");

    system("pause");
    return 0;
}