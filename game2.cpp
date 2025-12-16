#include "raylib.h"
#include "raymath.h"
#include <deque>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm> // Added for std::sort

// --- 參數設定 ---
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800;
const int MAP_SIZE = 3000;

const float BASE_SPEED = 5.0f;
const float BOOST_SPEED = 9.0f;
const int SEGMENT_SPACING = 12;
const float SNAKE_RADIUS = 18.0f;
const int START_LENGTH = 20;

// 體力系統
const float MAX_STAMINA = 100.0f;
const float STAMINA_DRAIN = 1.5f; // 衝刺消耗速度
const float STAMINA_REGEN = 0.5f; // 回復速度

const char* SAVE_FILE_PATH = "C:\\Users\\CNC-313\\Downloads\\snake_score.csv";

// 遊戲狀態
enum GameState {
    MENU,
    INPUT_NAME, // 新增：輸入名字狀態
    RECORDS,
    PLAYING,
    GAME_OVER,
    VICTORY
};

// --- 資料結構 ---

struct Star {
    Vector2 position;
    float size;
    float brightness; // 0.0 ~ 1.0
    float layerSpeed; // 視差滾動速度 (0.1 ~ 0.9)
};

struct FloatingText {
    Vector2 position;
    int value;
    float lifeTime; // 顯示時間
    float floatSpeed;
    Color color;
    bool active;
};

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float alpha;
    float size;
    Color color;
    bool active;
};

struct Food {
    Vector2 position;
    float radius;
    Color color;
    bool active;
};

struct Enemy {
    Vector2 position;
    float radius;
    int health;
    Color color;
    bool active;
    float spikeRotation; // 新增：尖刺旋轉角度
};

struct Snake {
    Vector2 position;
    float angle;
    float currentSpeed;
    int length;
    float radius;
    Color color;
    float stamina; // 新增體力
    std::deque<Vector2> trailPath;
};

struct ScoreRecord {
    std::string name;
    int score;
};

// --- 全域變數 ---
std::vector<Food> foods;
std::vector<Enemy> enemies;
std::vector<Particle> particles;
std::vector<FloatingText> combatTexts; // 新增戰鬥文字
std::vector<Star> stars; // 新增背景星星
std::vector<ScoreRecord> highScores; // 修改：儲存名字和分數

Snake player;
Camera2D camera = { 0 };
Vector2 portalPosition;
GameState gameState = MENU; // 預設進入主選單

int highScore = 0;
bool isNewRecord = false;
int currentLevel = 1; // 新增：當前關卡
char playerName[16] = "PLAYER"; // 新增：玩家名字緩衝區
int nameLetterCount = 6;

// --- 函式實作 ---

// 檔案讀寫
void LoadHighScore() {
    highScores.clear();
    std::ifstream file(SAVE_FILE_PATH);
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            try {
                size_t commaPos = line.find(',');
                if (commaPos != std::string::npos) {
                    std::string name = line.substr(0, commaPos);
                    int score = std::stoi(line.substr(commaPos + 1));
                    highScores.push_back({name, score});
                }
            } catch (...) {}
        }
        file.close();
    }
    
    // 排序並取前 5 名
    std::sort(highScores.begin(), highScores.end(), [](const ScoreRecord& a, const ScoreRecord& b) {
        return a.score > b.score;
    });
    if (highScores.size() > 5) highScores.resize(5);
    
    if (!highScores.empty()) highScore = highScores[0].score;
    else highScore = 0;
}

void SaveHighScore() {
    // 加入當前分數
    highScores.push_back({std::string(playerName), player.length});
    std::sort(highScores.begin(), highScores.end(), [](const ScoreRecord& a, const ScoreRecord& b) {
        return a.score > b.score;
    });
    if (highScores.size() > 5) highScores.resize(5);

    std::ofstream file(SAVE_FILE_PATH);
    if (file.is_open()) {
        for (const auto& record : highScores) {
            file << record.name << "," << record.score << "\n";
        }
        file.close();
    }
}

Color GetRandomColor() {
    return Color{
        (unsigned char)GetRandomValue(50, 255),
        (unsigned char)GetRandomValue(50, 255),
        (unsigned char)GetRandomValue(50, 255),
        255
    };
}

// 初始化星空
void InitStars() {
    stars.clear();
    for (int i = 0; i < 300; i++) {
        Star s;
        // 星星分佈在比地圖還大的範圍
        s.position = { (float)GetRandomValue(-MAP_SIZE, MAP_SIZE), (float)GetRandomValue(-MAP_SIZE, MAP_SIZE) };
        s.size = (float)GetRandomValue(1, 3) / 2.0f;
        s.brightness = (float)GetRandomValue(100, 255) / 255.0f;
        // 隨機層次：有些遠(動得慢)，有些近(動得快)
        s.layerSpeed = (float)GetRandomValue(1, 10) / 20.0f;
        stars.push_back(s);
    }
}

// 產生浮動文字
void SpawnFloatingText(Vector2 pos, int val, Color col) {
    FloatingText ft;
    ft.position = pos;
    ft.value = val;
    ft.lifeTime = 1.0f; // 顯示 1 秒
    ft.floatSpeed = 2.0f;
    ft.color = col;
    ft.active = true;
    combatTexts.push_back(ft);
}

void SpawnParticles(Vector2 pos, Color color, int count) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.position = pos;
        float angle = (float)GetRandomValue(0, 360) * DEG2RAD;
        float speed = (float)GetRandomValue(3, 8);
        p.velocity = { cosf(angle) * speed, sinf(angle) * speed };
        p.alpha = 1.0f;
        p.size = (float)GetRandomValue(3, 8);
        p.color = color;
        p.active = true;
        particles.push_back(p);
    }
}

void SpawnFood(int count) {
    for (int i = 0; i < count; i++) {
        Food f;
        f.position = { (float)GetRandomValue(-MAP_SIZE / 2, MAP_SIZE / 2), (float)GetRandomValue(-MAP_SIZE / 2, MAP_SIZE / 2) };
        f.radius = (float)GetRandomValue(8, 12);
        f.color = GetRandomColor();
        f.active = true;
        foods.push_back(f);
    }
}

void SpawnEnemies(int count) {
    for (int i = 0; i < count; i++) {
        Enemy e;
        float dist = 0;
        do {
            e.position = Vector2{ (float)GetRandomValue(-MAP_SIZE / 2, MAP_SIZE / 2), (float)GetRandomValue(-MAP_SIZE / 2, MAP_SIZE / 2) };
            dist = Vector2Distance(e.position, Vector2{ 0, 0 });
        } while (dist < 800);

        e.radius = (float)GetRandomValue(40, 80);
        e.health = (int)(e.radius / 3);
        e.color = Color{ 230, 41, 55, 255 };
        e.active = true;
        e.spikeRotation = (float)GetRandomValue(0, 360); // 初始化旋轉角度
        enemies.push_back(e);
    }
}

void ResetGame() {
    LoadHighScore();
    isNewRecord = false;

    player.position = { 0, 0 };
    player.angle = 0;
    player.length = START_LENGTH;
    player.radius = SNAKE_RADIUS;
    player.color = Color{ 0, 228, 48, 255 };
    player.currentSpeed = BASE_SPEED;
    player.stamina = MAX_STAMINA;
    player.trailPath.clear();

    for (int i = 0; i < player.length * SEGMENT_SPACING + 100; i++) {
        player.trailPath.push_front(Vector2{ -((float)i), 0 });
    }

    foods.clear();
    SpawnFood(300);

    enemies.clear();
    SpawnEnemies(60 + (currentLevel - 1) * 20); // 隨關卡增加敵人

    particles.clear();
    combatTexts.clear();
    InitStars(); // 每次開始重新分佈星星

    float randomAngle = GetRandomValue(0, 360) * DEG2RAD;
    float randomDist = GetRandomValue(1200, MAP_SIZE / 2 - 100);
    portalPosition.x = cosf(randomAngle) * randomDist;
    portalPosition.y = sinf(randomAngle) * randomDist;

    gameState = PLAYING;
}

// 繪製按鈕輔助函式 (回傳 true 代表被點擊)
bool DrawButton(const char* text, int centerX, int centerY, int width, int height) {
    Rectangle rect = { (float)(centerX - width / 2), (float)(centerY - height / 2), (float)width, (float)height };
    Vector2 mousePoint = GetMousePosition();
    bool isHover = CheckCollisionPointRec(mousePoint, rect);

    // 繪製按鈕背景
    if (isHover) {
        DrawRectangleRec(rect, LIGHTGRAY);
        DrawRectangleLinesEx(rect, 3, GREEN);
    }
    else {
        DrawRectangleRec(rect, DARKGRAY);
        DrawRectangleLinesEx(rect, 2, GRAY);
    }

    // 繪製文字 (置中)
    int textWidth = MeasureText(text, 30);
    DrawText(text, centerX - textWidth / 2, centerY - 15, 30, isHover ? DARKGREEN : WHITE);

    if (isHover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        return true;
    }
    return false;
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Snake RPG: Cosmic Edition");
    SetTargetFPS(60);

    LoadHighScore();
    InitStars(); // 主選單也要有星星背景

    while (!WindowShouldClose())
    {
        // ----------------------------------------------------------------------------------
        // Update Logic
        // ----------------------------------------------------------------------------------

        // 讓星星在主選單慢慢飄動 (視覺特效)
        if (gameState == MENU || gameState == RECORDS) {
            for (auto& s : stars) {
                s.position.x -= 0.2f;
                if (s.position.x < -MAP_SIZE) s.position.x = MAP_SIZE;
            }
        }

        if (gameState == PLAYING)
        {
            if (player.length > highScore) {
                highScore = player.length;
                isNewRecord = true;
            }

            // 1. 操控
            Vector2 direction = { 0, 0 };
            if (IsKeyDown(KEY_W)) direction.y -= 1;
            if (IsKeyDown(KEY_S)) direction.y += 1;
            if (IsKeyDown(KEY_A)) direction.x -= 1;
            if (IsKeyDown(KEY_D)) direction.x += 1;

            // 2. 加速與體力系統
            bool isBoosting = IsKeyDown(KEY_SPACE);
            if (isBoosting && player.stamina > 0) {
                player.currentSpeed = Lerp(player.currentSpeed, BOOST_SPEED, 0.1f);
                camera.zoom = Lerp(camera.zoom, 0.8f, 0.05f);
                player.stamina -= STAMINA_DRAIN;
                if (player.stamina < 0) player.stamina = 0;

                // 加速時產生小粒子特效
                if (GetRandomValue(0, 5) == 0) {
                    SpawnParticles(player.position, Fade(WHITE, 0.5f), 1);
                }
            }
            else {
                player.currentSpeed = Lerp(player.currentSpeed, BASE_SPEED, 0.1f);
                camera.zoom = Lerp(camera.zoom, 1.0f, 0.05f);
                // 回復體力
                if (player.stamina < MAX_STAMINA) {
                    player.stamina += STAMINA_REGEN;
                }
            }

            if (Vector2Length(direction) > 0) {
                float targetAngle = atan2f(direction.y, direction.x) * RAD2DEG;
                float currentAngleDeg = player.angle * RAD2DEG;
                float diff = targetAngle - currentAngleDeg;
                while (diff <= -180) diff += 360;
                while (diff > 180) diff -= 360;
                player.angle += (diff * 0.15f) * DEG2RAD;
            }

            Vector2 velocity = { cosf(player.angle) * player.currentSpeed, sinf(player.angle) * player.currentSpeed };
            player.position = Vector2Add(player.position, velocity);

            // 限制範圍
            if (player.position.x > MAP_SIZE / 2) player.position.x = MAP_SIZE / 2;
            if (player.position.x < -MAP_SIZE / 2) player.position.x = -MAP_SIZE / 2;
            if (player.position.y > MAP_SIZE / 2) player.position.y = MAP_SIZE / 2;
            if (player.position.y < -MAP_SIZE / 2) player.position.y = -MAP_SIZE / 2;

            player.trailPath.push_front(player.position);
            int maxHistory = player.length * SEGMENT_SPACING + 150;
            if (player.trailPath.size() > maxHistory) player.trailPath.pop_back();

            camera.offset = Vector2{ SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
            camera.target = player.position;

            // 星星視差移動 (根據玩家速度移動背景，製造深度感)
            for (auto& s : stars) {
                // 星星往反方向移動
                s.position = Vector2Subtract(s.position, Vector2Scale(velocity, s.layerSpeed));

                // 循環捲動邏輯 (讓星星永遠在玩家周圍)
                if (s.position.x < player.position.x - SCREEN_WIDTH) s.position.x += SCREEN_WIDTH * 2;
                if (s.position.x > player.position.x + SCREEN_WIDTH) s.position.x -= SCREEN_WIDTH * 2;
                if (s.position.y < player.position.y - SCREEN_HEIGHT) s.position.y += SCREEN_HEIGHT * 2;
                if (s.position.y > player.position.y + SCREEN_HEIGHT) s.position.y -= SCREEN_HEIGHT * 2;
            }

            // 碰撞檢測
            for (auto& f : foods) {
                if (f.active && CheckCollisionCircles(player.position, player.radius + 10, f.position, f.radius)) {
                    f.active = false;
                    player.length += 1;
                    SpawnParticles(f.position, f.color, 5);
                }
            }
            int activeFood = 0;
            for (const auto& f : foods) if (f.active) activeFood++;
            if (activeFood < 200) SpawnFood(20);

            for (auto& e : enemies) {
                if (e.active) {
                    e.spikeRotation += 2.0f; // 旋轉尖刺
                    if (CheckCollisionCircles(player.position, player.radius, e.position, e.radius)) {
                        if (player.length > e.health + 2) {
                            player.length -= e.health;
                            e.active = false;
                            SpawnParticles(e.position, e.color, 25);
                            player.currentSpeed = 1.0f; // 撞擊頓感

                            // 產生浮動文字
                            SpawnFloatingText(e.position, e.health, RED);

                        }
                        else {
                            gameState = GAME_OVER;
                            SaveHighScore();
                            SpawnParticles(player.position, player.color, 50);
                        }
                    }
                }
            }

            int startCheck = 25;
            for (size_t i = startCheck; i < player.trailPath.size(); i += 5) {
                if (i > player.length * SEGMENT_SPACING) break;
                if (CheckCollisionCircles(player.position, player.radius - 5, player.trailPath[i], player.radius - 8)) {
                    gameState = GAME_OVER;
                    SaveHighScore();
                }
            }

            if (CheckCollisionCircles(player.position, player.radius, portalPosition, 60)) {
                gameState = VICTORY;
                SaveHighScore();
            }

            // 更新粒子與文字
            for (auto& p : particles) {
                if (p.active) {
                    p.position = Vector2Add(p.position, p.velocity);
                    p.alpha -= 0.04f;
                    if (p.alpha <= 0) p.active = false;
                }
            }
            for (auto& t : combatTexts) {
                if (t.active) {
                    t.position.y -= t.floatSpeed; // 往上飄
                    t.lifeTime -= 0.02f;
                    if (t.lifeTime <= 0) t.active = false;
                }
            }
        }
        else if (gameState == GAME_OVER || gameState == VICTORY) {
            if (IsKeyPressed(KEY_ENTER)) {
                if (gameState == VICTORY) {
                    currentLevel++;
                    ResetGame(); // 進入下一關
                } else {
                    currentLevel = 1; // 重置關卡
                    gameState = MENU; // 回主選單
                }
            }
        }

        // ----------------------------------------------------------------------------------
        // Draw Logic
        // ----------------------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(Color{ 10, 10, 15, 255 }); // 極深藍色宇宙背景

        // 不論在哪個狀態都畫星星 (背景層)
        BeginMode2D(camera);
        if (gameState == MENU || gameState == RECORDS) {
            // 選單時鏡頭不動，讓星星自己飄
            Camera2D menuCam = { 0 };
            menuCam.zoom = 1.0f;
            EndMode2D(); // 關掉遊戲鏡頭
            BeginMode2D(menuCam); // 用固定鏡頭畫星星
        }

        for (const auto& s : stars) {
            Color starCol = Fade(WHITE, s.brightness);
            DrawCircleV(s.position, s.size, starCol);
        }

        if (gameState == PLAYING || gameState == GAME_OVER || gameState == VICTORY) {
            // 畫網格
            int gridStep = 150;
            int startX = (int)((player.position.x - SCREEN_WIDTH) / gridStep) * gridStep;
            int endX = (int)((player.position.x + SCREEN_WIDTH) / gridStep) * gridStep;
            int startY = (int)((player.position.y - SCREEN_HEIGHT) / gridStep) * gridStep;
            int endY = (int)((player.position.y + SCREEN_HEIGHT) / gridStep) * gridStep;
            for (int x = startX; x <= endX; x += gridStep) DrawLine(x, -MAP_SIZE, x, MAP_SIZE, Fade(WHITE, 0.05f));
            for (int y = startY; y <= endY; y += gridStep) DrawLine(-MAP_SIZE, y, MAP_SIZE, y, Fade(WHITE, 0.05f));

            DrawRectangleLines(-MAP_SIZE / 2, -MAP_SIZE / 2, MAP_SIZE, MAP_SIZE, RED);

            // 畫傳送門 (蘋果造型)
            float appleScale = 1.0f + sinf((float)GetTime() * 3) * 0.05f; // 呼吸效果
            float appleRadius = 60.0f * appleScale;

            // 蘋果梗
            DrawRectangle((int)portalPosition.x - 4, (int)portalPosition.y - (int)appleRadius - 15, 8, 25, BROWN);
            // 葉子
            DrawEllipse((int)portalPosition.x + 15, (int)portalPosition.y - (int)appleRadius - 5, 20, 10, GREEN);
            // 蘋果本體
            DrawCircleV(portalPosition, appleRadius, RED);
            // 蘋果光澤
            DrawCircle((int)portalPosition.x - (int)(appleRadius * 0.3f), (int)portalPosition.y - (int)(appleRadius * 0.3f), appleRadius * 0.25f, Fade(WHITE, 0.5f));
            
            DrawText("APPLE", (int)portalPosition.x - 40, (int)portalPosition.y - 10, 20, WHITE);

            for (const auto& f : foods) if (f.active) { DrawCircleV(f.position, f.radius, f.color); DrawCircleV(f.position, f.radius * 0.5f, WHITE); }

            for (const auto& e : enemies) if (e.active) {
                // 繪製尖刺球
                DrawCircleV(e.position, e.radius, e.color);
                
                // 畫尖刺
                int spikeCount = 8;
                for (int i = 0; i < spikeCount; i++) {
                    float angle = (e.spikeRotation + i * (360.0f / spikeCount)) * DEG2RAD;
                    Vector2 p1 = { e.position.x + cosf(angle) * (e.radius + 15), e.position.y + sinf(angle) * (e.radius + 15) };
                    Vector2 p2 = { e.position.x + cosf(angle - 0.2f) * e.radius, e.position.y + sinf(angle - 0.2f) * e.radius };
                    Vector2 p3 = { e.position.x + cosf(angle + 0.2f) * e.radius, e.position.y + sinf(angle + 0.2f) * e.radius };
                    DrawTriangle(p1, p3, p2, DARKGRAY);
                }

                DrawCircleLines((int)e.position.x, (int)e.position.y, e.radius, WHITE);
                DrawText(TextFormat("-%d", e.health), (int)e.position.x - 10, (int)e.position.y - 10, 30, WHITE);
            }

            for (const auto& p : particles) if (p.active) DrawCircleV(p.position, p.size, Fade(p.color, p.alpha));

            // 畫蛇
            for (int i = player.length; i > 0; i--) {
                int index = i * SEGMENT_SPACING;
                if (index < player.trailPath.size()) {
                    Vector2 pos = player.trailPath[index];
                    Color bodyCol = player.color;
                    if (i % 2 == 0) bodyCol = ColorBrightness(player.color, -0.2f);
                    DrawCircleV(pos, player.radius, bodyCol);
                }
            }
            DrawCircleV(player.position, player.radius, player.color);
            DrawCircleV(player.position, player.radius + 3, Fade(player.color, 0.3f));
            Vector2 eyeL = { cosf(player.angle - 0.6f) * 10, sinf(player.angle - 0.6f) * 10 };
            Vector2 eyeR = { cosf(player.angle + 0.6f) * 10, sinf(player.angle + 0.6f) * 10 };
            DrawCircleV(Vector2Add(player.position, eyeL), 5, WHITE); DrawCircleV(Vector2Add(player.position, eyeR), 5, WHITE);
            DrawCircleV(Vector2Add(player.position, eyeL), 2, BLACK); DrawCircleV(Vector2Add(player.position, eyeR), 2, BLACK);

            // 畫浮動文字 (Combat Text)
            for (const auto& t : combatTexts) {
                if (t.active) {
                    DrawText(TextFormat("-%d", t.value), (int)t.position.x, (int)t.position.y, 30, Fade(t.color, t.lifeTime));
                }
            }
        }
        EndMode2D();

        // ----------------------------------------------------------------------------------
        // UI Logic (介面層)
        // ----------------------------------------------------------------------------------

        if (gameState == MENU) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.4f));

            const char* title = "SNAKE RPG";
            DrawText(title, SCREEN_WIDTH / 2 - MeasureText(title, 80) / 2, 150, 80, GREEN);
            DrawText("COSMIC EDITION", SCREEN_WIDTH / 2 - MeasureText("COSMIC EDITION", 30) / 2, 230, 30, LIGHTGRAY);

            if (DrawButton("START GAME", SCREEN_WIDTH / 2, 350, 250, 60)) {
                gameState = INPUT_NAME; // 改為進入輸入名字狀態
                nameLetterCount = 0;
                for(int i=0; i<16; i++) playerName[i] = '\0'; // 清空名字
            }
            if (DrawButton("RECORDS", SCREEN_WIDTH / 2, 430, 250, 60)) {
                LoadHighScore();
                gameState = RECORDS;
            }
            if (DrawButton("QUIT", SCREEN_WIDTH / 2, 510, 250, 60)) {
                CloseWindow();
                return 0;
            }
        }
        else if (gameState == INPUT_NAME) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.8f));
            DrawText("ENTER YOUR NAME", SCREEN_WIDTH / 2 - 150, 200, 40, GREEN);

            // 繪製輸入框
            Rectangle textBox = { SCREEN_WIDTH / 2.0f - 150, 300, 300, 50 };
            DrawRectangleRec(textBox, LIGHTGRAY);
            DrawRectangleLines((int)textBox.x, (int)textBox.y, (int)textBox.width, (int)textBox.height, GREEN);
            
            DrawText(playerName, (int)textBox.x + 10, (int)textBox.y + 10, 40, MAROON);
            
            // 閃爍游標
            if ((GetTime() * 2.0f) - (int)(GetTime() * 2.0f) < 0.5f) {
                DrawText("_", (int)textBox.x + 10 + MeasureText(playerName, 40), (int)textBox.y + 10, 40, MAROON);
            }

            DrawText(TextFormat("CHARS: %i/15", nameLetterCount), SCREEN_WIDTH / 2 - 150, 360, 20, DARKGRAY);

            if (DrawButton("CONFIRM", SCREEN_WIDTH / 2, 450, 200, 50)) {
                if (nameLetterCount > 0) {
                    ResetGame();
                }
            }
            if (DrawButton("BACK", SCREEN_WIDTH / 2, 520, 200, 50)) {
                gameState = MENU;
            }

            // 處理輸入
            int key = GetCharPressed();
            while (key > 0) {
                if ((key >= 32) && (key <= 125) && (nameLetterCount < 15)) {
                    playerName[nameLetterCount] = (char)key;
                    playerName[nameLetterCount + 1] = '\0';
                    nameLetterCount++;
                }
                key = GetCharPressed();
            }

            if (IsKeyPressed(KEY_BACKSPACE)) {
                nameLetterCount--;
                if (nameLetterCount < 0) nameLetterCount = 0;
                playerName[nameLetterCount] = '\0';
            }
            
            if (IsKeyPressed(KEY_ENTER) && nameLetterCount > 0) {
                ResetGame();
            }
        }
        else if (gameState == RECORDS) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6f));
            DrawText("HALL OF FAME", SCREEN_WIDTH / 2 - 150, 150, 50, GOLD);

            // 顯示紀錄列表
            DrawRectangle(SCREEN_WIDTH / 2 - 200, 230, 400, 250, Fade(WHITE, 0.1f));
            
            for (size_t i = 0; i < highScores.size(); i++) {
                Color rankColor = WHITE;
                if (i == 0) rankColor = GOLD;
                else if (i == 1) rankColor = LIGHTGRAY; // 銀
                else if (i == 2) rankColor = BROWN;     // 銅
                
                DrawText(TextFormat("%d. %s - %d", i + 1, highScores[i].name.c_str(), highScores[i].score), SCREEN_WIDTH / 2 - 150, 250 + i * 40, 30, rankColor);
            }

            DrawText(TextFormat("File: %s", SAVE_FILE_PATH), 20, SCREEN_HEIGHT - 30, 20, GRAY);

            if (DrawButton("BACK", SCREEN_WIDTH / 2, 550, 200, 50)) {
                gameState = MENU;
            }
        }
        else if (gameState == PLAYING) {
            // HUD
            DrawText(TextFormat("PLAYER: %s", playerName), 30, 180, 20, WHITE); // 顯示玩家名字
            DrawText(TextFormat("LEVEL: %i", currentLevel), 30, 140, 30, ORANGE); // 顯示關卡
            DrawText(TextFormat("LENGTH: %i", player.length), 30, 30, 40, GREEN);

            // 體力條 (Stamina Bar)
            DrawText("STAMINA", 30, 80, 20, SKYBLUE);
            DrawRectangle(30, 105, 200, 20, DARKGRAY);
            DrawRectangle(30, 105, (int)(player.stamina / MAX_STAMINA * 200), 20, SKYBLUE);
            DrawRectangleLines(30, 105, 200, 20, GRAY);

            // 最高分
            DrawText(TextFormat("BEST: %i", highScore), SCREEN_WIDTH - 230, 30, 30, GOLD);
            if (isNewRecord) DrawText("NEW RECORD!", SCREEN_WIDTH - 230, 70, 20, RED);

            // 導航
            Vector2 toPortal = Vector2Subtract(portalPosition, player.position);
            float angleToPortal = atan2f(toPortal.y, toPortal.x) * RAD2DEG;
            float distToPortal = Vector2Length(toPortal);
            float arrowX = SCREEN_WIDTH / 2.0f;
            float arrowY = SCREEN_HEIGHT - 60.0f;
            DrawText("TARGET", (int)arrowX - 30, (int)arrowY + 30, 20, SKYBLUE);
            DrawText(TextFormat("%dm", (int)distToPortal / 10), (int)arrowX - 20, (int)arrowY + 50, 20, GRAY);
            Vector2 p1 = { arrowX + cosf(angleToPortal * DEG2RAD) * 30, arrowY + sinf(angleToPortal * DEG2RAD) * 30 };
            Vector2 p2 = { arrowX + cosf((angleToPortal + 140) * DEG2RAD) * 15, arrowY + sinf((angleToPortal + 140) * DEG2RAD) * 15 };
            Vector2 p3 = { arrowX + cosf((angleToPortal - 140) * DEG2RAD) * 15, arrowY + sinf((angleToPortal - 140) * DEG2RAD) * 15 };
            DrawTriangle(p1, p3, p2, RED);
        }
        else if (gameState == GAME_OVER) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.7f));
            DrawText("YOU DIED", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 - 60, 60, RED);
            DrawText(TextFormat("Final Length: %d", player.length), SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 10, 30, WHITE);
            if (DrawButton("MAIN MENU", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 80, 200, 50)) gameState = MENU;
        }
        else if (gameState == VICTORY) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(WHITE, 0.8f));
            DrawText("MISSION COMPLETE", SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 60, 60, GOLD);
            DrawText(TextFormat("Final Length: %d", player.length), SCREEN_WIDTH / 2 - 110, SCREEN_HEIGHT / 2 + 10, 30, DARKGRAY);
            
            if (DrawButton("NEXT LEVEL", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 80, 200, 50)) {
                currentLevel++;
                ResetGame();
            }
            if (DrawButton("MAIN MENU", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 150, 200, 50)) {
                currentLevel = 1;
                gameState = MENU;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}