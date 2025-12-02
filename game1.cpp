#include "raylib.h"

// --- 定義常數 (視窗變大了！) ---
const int screenWidth = 1200;  // 原本 800 -> 1200
const int screenHeight = 675;  // 原本 450 -> 675
const int groundHeight = 550;  // 地面高度也往下移

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Dino Run - Big Version");
    SetTargetFPS(60);

    // --- 載入圖片 (保持你的路徑) ---
    Texture2D dinoTexture = LoadTexture("C:\\Users\\CNC-313\\Downloads\\dino.png");
    Texture2D cactusTexture = LoadTexture("C:\\Users\\CNC-313\\Downloads\\cactus.png");

    // --- 遊戲變數 ---

    // 1. 小恐龍變大了 (寬高從 40,40 -> 80,80)
    // 雖然圖片本身沒變，但我們會把它拉大顯示
    Rectangle dino = { 100, groundHeight - 80, 80, 80 };

    // 2. 物理數據調整 (因為畫面變大，力量要大一點才自然)
    float velocity = 0;
    float gravity = 1.0f;      // 重力加強 (原本 0.6)
    float jumpPower = -22.0f;  // 跳躍力加強 (原本 -12)
    bool isOnGround = true;

    // 3. 障礙物變大了 (寬高從 30,40 -> 60,80)
    Rectangle cactus = { screenWidth, groundHeight - 80, 60, 80 };
    bool gameOver = false;
    int score = 0;
    int speed = 10; // 移動速度變快一點 (原本 6)

    while (!WindowShouldClose())
    {
        // --- 邏輯更新 ---
        if (!gameOver)
        {
            // 跳躍
            if (IsKeyPressed(KEY_SPACE) && isOnGround) {
                velocity = jumpPower;
                isOnGround = false;
            }

            // 重力
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

            // 障礙物移動
            cactus.x -= speed;
            if (cactus.x < -60) { // 因為障礙物變寬了，所以要跑到更左邊(-60)才重置
                cactus.x = screenWidth + GetRandomValue(0, 300);
                score++;

                // (選做) 每得 5 分速度變快一點點
                if (score % 5 == 0) speed += 1;
            }

            // 碰撞
            // 為了讓手感好一點，我們可以把判定框稍微縮小一點點 (縮進去 10 像素)
            // 這樣才不會覺得「明明沒碰到卻死掉了」
            Rectangle collisionDino = { dino.x + 10, dino.y + 10, dino.width - 20, dino.height - 20 };
            Rectangle collisionCactus = { cactus.x + 10, cactus.y + 10, cactus.width - 20, cactus.height - 20 };

            if (CheckCollisionRecs(collisionDino, collisionCactus)) {
                gameOver = true;
            }
        }
        else {
            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                cactus.x = screenWidth;
                score = 0;
                speed = 10; // 重置速度
                dino.y = groundHeight - dino.height;
                velocity = 0;
            }
        }

        // --- 繪圖 ---
        BeginDrawing();

        ClearBackground(RAYWHITE);

        // 畫地面 (線條加粗一點)
        DrawLineEx({ 0, (float)groundHeight }, { (float)screenWidth, (float)groundHeight }, 5.0f, DARKGRAY);

        // --- 畫小恐龍 ---
        if (dinoTexture.id > 0) {
            Rectangle sourceRecDino = { 0.0f, 0.0f, (float)dinoTexture.width, (float)dinoTexture.height };
            DrawTexturePro(dinoTexture, sourceRecDino, dino, { 0, 0 }, 0.0f, WHITE);
        }
        else {
            DrawRectangleRec(dino, MAROON);
        }

        // --- 畫障礙物 ---
        if (cactusTexture.id > 0) {
            Rectangle sourceRecCactus = { 0.0f, 0.0f, (float)cactusTexture.width, (float)cactusTexture.height };
            DrawTexturePro(cactusTexture, sourceRecCactus, cactus, { 0, 0 }, 0.0f, WHITE);
        }
        else {
            DrawRectangleRec(cactus, DARKGREEN);
        }

        // 畫分數 (字體放大)
        DrawText(TextFormat("Score: %04d", score), 30, 30, 40, LIGHTGRAY); // 字體 20 -> 40

        if (gameOver) {
            DrawText("GAME OVER", screenWidth / 2 - 200, screenHeight / 2 - 50, 80, RED); // 字體 40 -> 80
            DrawText("Press 'R' to Restart", screenWidth / 2 - 150, screenHeight / 2 + 50, 30, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadTexture(dinoTexture);
    UnloadTexture(cactusTexture);

    CloseWindow();

    return 0;
}