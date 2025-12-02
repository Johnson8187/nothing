#include "raylib.h"

const int screenWidth = 1200;
const int screenHeight = 675;
const int groundHeight = 550;

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Dino Run - Fun Version");
    SetTargetFPS(60);

    Texture2D dinoTexture = LoadTexture("C:\\Users\\CNC-313\\Downloads\\dino.png");
    Texture2D cactusTexture = LoadTexture("C:\\Users\\CNC-313\\Downloads\\cactus.png");

    Rectangle dino = { 100, groundHeight - 80, 80, 80 };

    float velocity = 0;
    float gravity = 1.0f;
    float jumpPower = -22.0f;
    bool isOnGround = true;

    // --- 新增：障礙物會有不同的大小 ---
    // 我們用一個變數來存當前的寬高，每次重生時隨機改變
    float cactusWidth = 60;
    float cactusHeight = 80;
    Rectangle cactus = { screenWidth, groundHeight - cactusHeight, cactusWidth, cactusHeight };

    bool gameOver = false;
    int score = 0;
    // --- 新增：最高分變數 ---
    int highScore = 0;

    int speed = 10;

    while (!WindowShouldClose())
    {
        if (!gameOver)
        {
            if (IsKeyPressed(KEY_SPACE) && isOnGround) {
                velocity = jumpPower;
                isOnGround = false;
            }

            // 支援「長按跳得更高」的小手感優化 (選用)
            // 如果玩家在上升途中放開空白鍵，重力加重，讓跳躍變短
            if (IsKeyReleased(KEY_SPACE) && velocity < 0) {
                velocity *= 0.5f;
            }

            dino.y += velocity;
            if (dino.y + dino.height < groundHeight) {
                velocity += gravity;
                isOnGround = false;
            }
            else {
                dino.y = groundHeight - dino.height;
                velocity = 0;
                isOnGround = true;
            }

            cactus.x -= speed;

            // --- 修改：障礙物重生邏輯 ---
            if (cactus.x < -100) {
                cactus.x = screenWidth + GetRandomValue(0, 400); // 隨機間距更明顯

                // 隨機生成不同形狀的仙人掌 (瘦高 或 矮胖)
                int type = GetRandomValue(0, 2);
                if (type == 0) { cactusWidth = 50; cactusHeight = 90; } // 瘦高
                else if (type == 1) { cactusWidth = 90; cactusHeight = 60; } // 矮胖
                else { cactusWidth = 60; cactusHeight = 80; } // 正常

                // 更新判定框大小和位置(因為高度變了，Y軸也要修正)
                cactus.width = cactusWidth;
                cactus.height = cactusHeight;
                cactus.y = groundHeight - cactusHeight;

                score++;
                if (score % 5 == 0) speed += 1;
            }

            Rectangle collisionDino = { dino.x + 15, dino.y + 15, dino.width - 30, dino.height - 30 };
            Rectangle collisionCactus = { cactus.x + 10, cactus.y + 10, cactus.width - 20, cactus.height - 20 };

            if (CheckCollisionRecs(collisionDino, collisionCactus)) {
                gameOver = true;
                // --- 新增：更新最高分 ---
                if (score > highScore) {
                    highScore = score;
                }
            }
        }
        else {
            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                cactus.x = screenWidth;
                score = 0;
                speed = 10;
                dino.y = groundHeight - dino.height;
                velocity = 0;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawLineEx({ 0, (float)groundHeight }, { (float)screenWidth, (float)groundHeight }, 5.0f, DARKGRAY);

        // --- 新增：畫雲朵 (簡單的裝飾) ---
        // 這裡用簡單的圓形畫幾朵雲，讓背景不那麼空
        DrawCircle(200, 100, 30, Fade(SKYBLUE, 0.5f));
        DrawCircle(240, 100, 40, Fade(SKYBLUE, 0.5f));
        DrawCircle(280, 100, 30, Fade(SKYBLUE, 0.5f));

        DrawCircle(900, 150, 40, Fade(SKYBLUE, 0.5f));
        DrawCircle(950, 160, 50, Fade(SKYBLUE, 0.5f));

        if (dinoTexture.id > 0) {
            Rectangle sourceRecDino = { 0.0f, 0.0f, (float)dinoTexture.width, (float)dinoTexture.height };
            DrawTexturePro(dinoTexture, sourceRecDino, dino, { 0, 0 }, 0.0f, WHITE);
        }
        else {
            DrawRectangleRec(dino, MAROON);
        }

        if (cactusTexture.id > 0) {
            Rectangle sourceRecCactus = { 0.0f, 0.0f, (float)cactusTexture.width, (float)cactusTexture.height };
            DrawTexturePro(cactusTexture, sourceRecCactus, cactus, { 0, 0 }, 0.0f, WHITE);
        }
        else {
            // 如果沒圖片，我們可以根據隨機大小畫出不同形狀的綠色方塊
            DrawRectangleRec(cactus, DARKGREEN);
        }

        // --- 新增：顯示最高分 ---
        DrawText(TextFormat("HI: %04d", highScore), screenWidth - 200, 30, 30, GRAY);
        DrawText(TextFormat("Score: %04d", score), 30, 30, 40, LIGHTGRAY);

        if (gameOver) {
            DrawText("GAME OVER", screenWidth / 2 - 200, screenHeight / 2 - 50, 80, RED);
            DrawText("Press 'R' to Restart", screenWidth / 2 - 150, screenHeight / 2 + 50, 30, DARKGRAY);
            // 顯示這次的分數
            DrawText(TextFormat("Final Score: %d", score), screenWidth / 2 - 100, screenHeight / 2 + 100, 30, BLACK);
        }

        EndDrawing();
    }

    UnloadTexture(dinoTexture);
    UnloadTexture(cactusTexture);
    CloseWindow();
    return 0;
}