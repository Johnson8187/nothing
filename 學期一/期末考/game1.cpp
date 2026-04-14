#include "raylib.h"
#include <vector> // 為了使用 vector 管理粒子

// --- 定義常數 ---
const int screenWidth = 1200;
const int screenHeight = 675;
const int groundHeight = 550;

// 定義道具與敵人
enum ItemType { NONE, DRINK, COIN };
enum EnemyType { CACTUS, BIRD };

// --- 粒子結構 (爆炸碎片) ---
struct Particle {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float rotationalSpeed;
    Color color;
    float life; // 壽命 (1.0 -> 0.0)
};

int main(void)
{
    InitWindow(screenWidth, screenHeight, "Dino Run - HARDCORE MODE");
    SetTargetFPS(60);

    // --- 資源路徑 ---
    const char* rootPath = "C:\\Users\\CNC-313\\Downloads\\";

    Texture2D bgTexture = LoadTexture(TextFormat("%sbg.png", rootPath));
    Texture2D dinoIdle = LoadTexture(TextFormat("%sdino.png", rootPath));
    Texture2D dinoRun1 = LoadTexture(TextFormat("%sdino_2.png", rootPath));
    Texture2D dinoRun2 = LoadTexture(TextFormat("%sdino_3.png", rootPath));
    
    Texture2D cactusTexture = LoadTexture(TextFormat("%scactus.png", rootPath));
    // 假設你有 bird.png，沒有也沒關係，會用色塊代替
    Texture2D birdTexture = LoadTexture(TextFormat("%sbird.png", rootPath)); 

    Texture2D drinkTexture = LoadTexture(TextFormat("%sdrink.png", rootPath));
    Texture2D coinTexture = LoadTexture(TextFormat("%scoin.png", rootPath));

    // --- 遊戲變數 ---
    // 恐龍初始設定
    Rectangle dino = { 100, groundHeight - 80, 80, 80 }; 
    // 下蹲時的高度
    float duckHeight = 40.0f;
    float standHeight = 80.0f;
    
    float velocity = 0;
    float gravity = 1.0f;
    float jumpPower = -22.0f;
    bool isOnGround = true;
    bool isDucking = false; // 是否正在下蹲

    // 動畫與背景
    float frameTimer = 0.0f;
    int currentFrame = 0;
    float bgX = 0.0f;

    // --- 敵人系統 (障礙物) ---
    Rectangle enemyRec = { screenWidth, groundHeight - 80, 60, 80 };
    EnemyType currentEnemyType = CACTUS;

    // --- 粒子系統 (爆炸效果) ---
    std::vector<Particle> particles;

    // --- 道具變數 ---
    Rectangle itemRec = { -100, 0, 50, 50 }; 
    ItemType currentItemType = NONE;
    bool itemActive = false;

    // --- Buff 效果 ---
    float shelterTimer = 0.0f;
    float baseSpeed = 10.0f;
    float speedBoost = 8.0f; // 喝飲料加速更多！更難控制！
    float doubleScoreTimer = 0.0f;

    bool gameOver = false;
    float scoreFloat = 0;
    
    while (!WindowShouldClose())
    {
        // 速度計算
        float currentSpeed = (shelterTimer > 0) ? (baseSpeed + speedBoost) : baseSpeed;
        float bgSpeed = currentSpeed * 0.2f; 

        if (!gameOver)
        {
            // 1. 背景與粒子更新
            bgX -= bgSpeed;
            if (bgX <= -screenWidth) bgX = 0;

            // 更新所有爆炸粒子
            for (int i = 0; i < particles.size(); i++) {
                particles[i].position.x += particles[i].velocity.x;
                particles[i].position.y += particles[i].velocity.y;
                particles[i].rotation += particles[i].rotationalSpeed;
                particles[i].life -= 0.02f; // 慢慢消失
                
                // 如果壽命到了，移除粒子
                if (particles[i].life <= 0) {
                    particles.erase(particles.begin() + i);
                    i--;
                }
            }

            // 2. 玩家控制 (跳躍 & 下蹲)
            
            // 下蹲邏輯 (按住下鍵)
            if (IsKeyDown(KEY_DOWN) && isOnGround) {
                isDucking = true;
                dino.height = duckHeight; // 變矮
                dino.y = groundHeight - duckHeight; // 貼地
            } else {
                // 如果原本是蹲著，現在放開了，要變回站立
                if (isDucking) {
                    dino.y = groundHeight - standHeight;
                }
                isDucking = false;
                dino.height = standHeight; // 恢復身高
            }

            // 跳躍邏輯 (不能在下蹲時跳)
            if (IsKeyPressed(KEY_SPACE) && isOnGround && !isDucking) {
                velocity = jumpPower;
                isOnGround = false;
            }

            // 物理重力
            if (!isOnGround) {
                dino.y += velocity;
                velocity += gravity;
                // 落地判定
                if (dino.y + dino.height >= groundHeight) {
                    dino.y = groundHeight - dino.height;
                    velocity = 0;
                    isOnGround = true;
                }
            }
            
            // 跑步動畫
            if (isOnGround) {
                frameTimer += GetFrameTime();
                if (frameTimer >= (0.1f * (10.0f / currentSpeed))) { 
                    frameTimer = 0.0f;
                    currentFrame = !currentFrame;
                }
            }

            // 3. 敵人移動與生成
            enemyRec.x -= currentSpeed;
            if (enemyRec.x < -100) {
                // 重置敵人
                enemyRec.x = screenWidth + GetRandomValue(200, 500);
                scoreFloat += 1;
                if ((int)scoreFloat % 10 == 0) baseSpeed += 0.5f;

                // 隨機決定是 仙人掌(地面) 還是 鳥(空中)
                // 30% 機率出鳥
                if (GetRandomValue(0, 100) < 30) {
                    currentEnemyType = BIRD;
                    enemyRec.width = 60;
                    enemyRec.height = 40; 
                    // 鳥的高度：剛好會撞到頭，必須下蹲
                    enemyRec.y = groundHeight - 90; 
                } else {
                    currentEnemyType = CACTUS;
                    enemyRec.width = 60;
                    enemyRec.height = 80;
                    enemyRec.y = groundHeight - 80;
                }
            }

            // 4. 道具生成 (變得更稀有)
            if (!itemActive) {
                // 機率降低到 0.5% (原本約 2%)，且不能跟敵人太近
                if (enemyRec.x < screenWidth - 500 && GetRandomValue(0, 1000) < 5) { 
                    itemActive = true;
                    itemRec.x = screenWidth + 50;
                    currentItemType = (GetRandomValue(0, 1) == 0) ? DRINK : COIN;
                    
                    if (currentItemType == DRINK) itemRec.y = groundHeight - 60; // 地上
                    else itemRec.y = groundHeight - 150; // 天上
                }
            } else {
                itemRec.x -= currentSpeed;
                if (itemRec.x < -50) itemActive = false;

                if (CheckCollisionRecs(dino, itemRec)) {
                    if (currentItemType == DRINK) shelterTimer = 5.0f; // 5秒無敵
                    else if (currentItemType == COIN) doubleScoreTimer = 8.0f;
                    itemActive = false;
                }
            }

            // 計時器倒數
            if (shelterTimer > 0) shelterTimer -= GetFrameTime();
            if (doubleScoreTimer > 0) doubleScoreTimer -= GetFrameTime();
            float scoreMultiplier = (doubleScoreTimer > 0) ? 2.0f : 1.0f;
            scoreFloat += 0.1f * scoreMultiplier;

            // 5. 碰撞與死亡判定
            // 修正 hitbox
            Rectangle collisionDino = { dino.x + 10, dino.y + 10, dino.width - 20, dino.height - 20 };
            Rectangle collisionEnemy = { enemyRec.x + 10, enemyRec.y + 10, enemyRec.width - 20, enemyRec.height - 20 };

            if (CheckCollisionRecs(collisionDino, collisionEnemy)) {
                if (shelterTimer > 0) {
                    // --- 無敵模式：衝撞爆破！ ---
                    // 重置敵人
                    enemyRec.x = screenWidth + GetRandomValue(300, 600);
                    scoreFloat += 50; // 撞碎敵人加分！
                    
                    // 生成爆炸粒子
                    for (int i = 0; i < 20; i++) {
                        Particle p;
                        p.position = { collisionEnemy.x + collisionEnemy.width/2, collisionEnemy.y + collisionEnemy.height/2 };
                        p.velocity = { (float)GetRandomValue(-10, 10), (float)GetRandomValue(-15, 5) };
                        p.rotation = (float)GetRandomValue(0, 360);
                        p.rotationalSpeed = (float)GetRandomValue(-10, 10);
                        p.color = (currentEnemyType == CACTUS) ? DARKGREEN : PURPLE; // 仙人掌綠色，鳥紫色
                        p.life = 1.0f;
                        particles.push_back(p);
                    }
                } 
                else {
                    // --- 普通模式：死亡 ---
                    gameOver = true;
                }
            }
        }
        else {
            if (IsKeyPressed(KEY_R)) {
                gameOver = false;
                enemyRec.x = screenWidth;
                scoreFloat = 0;
                baseSpeed = 10.0f;
                dino.y = groundHeight - 80;
                dino.height = 80;
                isDucking = false;
                velocity = 0;
                shelterTimer = 0;
                doubleScoreTimer = 0;
                particles.clear();
            }
        }

        // --- 繪圖 ---
        BeginDrawing();
        ClearBackground(RAYWHITE); 

        // 畫背景
        Rectangle sourceBg = { 0, 0, (float)bgTexture.width, (float)bgTexture.height };
        DrawTexturePro(bgTexture, sourceBg, { bgX, 0, screenWidth, screenHeight }, {0,0}, 0, WHITE);
        DrawTexturePro(bgTexture, sourceBg, { bgX + screenWidth, 0, screenWidth, screenHeight }, {0,0}, 0, WHITE);

        // 畫恐龍
        // 如果無敵，產生殘影效果
        if (shelterTimer > 0) {
             DrawText("RAMPAGE MODE!!", dino.x - 20, dino.y - 40, 20, RED);
             DrawRectangleRec({dino.x-5, dino.y, dino.width, dino.height}, Fade(RED, 0.3f));
        }

        Texture2D currentDinoTex;
        if (isDucking) {
            // 如果蹲下，我們將原本的跑步圖「壓扁」來畫，或者你可以準備一張 dino_duck.png
            currentDinoTex = dinoRun1; // 暫時用跑步圖
            // 這裡不做任何事，因為 dino 的 height 已經變矮了，DrawTexturePro 會自動壓扁圖片
        } else {
             if (isOnGround) currentDinoTex = (currentFrame == 0) ? dinoRun1 : dinoRun2;
             else currentDinoTex = dinoIdle;
        }
        
        // 畫出恐龍本體
        if (currentDinoTex.id > 0) {
             DrawTexturePro(currentDinoTex, {0,0,(float)currentDinoTex.width,(float)currentDinoTex.height}, dino, {0,0}, 0, WHITE);
        } else {
             DrawRectangleRec(dino, MAROON);
        }

        // 畫敵人
        if (currentEnemyType == CACTUS) {
            if (cactusTexture.id > 0) DrawTexturePro(cactusTexture, {0,0,(float)cactusTexture.width,(float)cactusTexture.height}, enemyRec, {0,0}, 0, WHITE);
            else DrawRectangleRec(enemyRec, DARKGREEN);
        } 
        else if (currentEnemyType == BIRD) {
            if (birdTexture.id > 0) DrawTexturePro(birdTexture, {0,0,(float)birdTexture.width,(float)birdTexture.height}, enemyRec, {0,0}, 0, WHITE);
            else DrawRectangleRec(enemyRec, PURPLE); // 沒有鳥的圖就畫紫色方塊
        }

        // 畫道具
        if (itemActive) {
            Texture2D texItem = (currentItemType == DRINK) ? drinkTexture : coinTexture;
            if (texItem.id > 0) DrawTexturePro(texItem, {0,0,(float)texItem.width,(float)texItem.height}, itemRec, {0,0}, 0, WHITE);
            else DrawRectangleRec(itemRec, YELLOW);
        }

        // 畫粒子 (爆炸碎片)
        for (const auto& p : particles) {
            Rectangle pRect = { p.position.x, p.position.y, 10, 10 };
            DrawRectanglePro(pRect, {5,5}, p.rotation, Fade(p.color, p.life));
        }

        // UI
        Color scoreColor = (doubleScoreTimer > 0) ? GOLD : LIGHTGRAY;
        DrawText(TextFormat("Score: %05d", (int)scoreFloat), 30, 30, 40, scoreColor);
        
        if (gameOver) {
            DrawText("GAME OVER", screenWidth/2 - 200, screenHeight/2 - 50, 80, RED);
            DrawText("Press 'R' to Restart", screenWidth/2 - 150, screenHeight/2 + 50, 30, DARKGRAY);
        }

        EndDrawing();
    }

    UnloadTexture(bgTexture);
    UnloadTexture(dinoIdle);
    UnloadTexture(dinoRun1);
    UnloadTexture(dinoRun2);
    UnloadTexture(cactusTexture);
    UnloadTexture(birdTexture);
    UnloadTexture(drinkTexture);
    UnloadTexture(coinTexture);

    CloseWindow();
    return 0;
}