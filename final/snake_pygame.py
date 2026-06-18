import pygame
import random

# 一、遊戲畫面與初始化 (5%)
pygame.init()
win = pygame.display.set_mode((600, 600))
pygame.display.set_caption("貪食蛇")
clock = pygame.time.Clock()
font = pygame.font.SysFont("consolas", 30)

# 遊戲變數
snake = [[300, 300]]  # 蛇身座標列表，首位為蛇頭
dx, dy = 20, 0        # 移動方向 (預設向右，直接開始)
food = [random.randint(0, 29) * 20, random.randint(0, 29) * 20]
score = 0
game_over = False

# 主遊戲循環
while True:
    # 二、蛇的移動控制 (8%) - 鍵盤事件監聽
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            exit()
        elif event.type == pygame.KEYDOWN and not game_over:
            if event.key == pygame.K_UP and dy == 0:
                dx, dy = 0, -20
            elif event.key == pygame.K_DOWN and dy == 0:
                dx, dy = 0, 20
            elif event.key == pygame.K_LEFT and dx == 0:
                dx, dy = -20, 0
            elif event.key == pygame.K_RIGHT and dx == 0:
                dx, dy = 20, 0

    if not game_over and (dx != 0 or dy != 0):
        # 計算前進後的新蛇頭座標
        new_head = [snake[0][0] + dx, snake[0][1] + dy]
        
        # 四、碰撞判斷 (8%) - 撞到邊界或撞到自己
        if (new_head[0] < 0 or new_head[0] >= 600 or new_head[1] < 0 or new_head[1] >= 600 
                or new_head in snake):
            game_over = True
        else:
            # 三、食物生成與吃食物 (8%)
            snake.insert(0, new_head)
            if new_head == food:
                score += 1
                food = [random.randint(0, 29) * 20, random.randint(0, 29) * 20]
            else:
                snake.pop()  # 沒有吃到食物，則移除蛇尾以維持原長度

    # 畫面繪製 - 背景
    win.fill((30, 30, 30))
    
    # 畫蛇：頭部使用青藍色 (0, 173, 181)，身體使用深灰色 (57, 62, 70)
    for i, s in enumerate(snake):
        pygame.draw.rect(win, (0, 173, 181) if i == 0 else (57, 62, 70), (s[0], s[1], 20, 20))
        
    # 畫食物 (粉紅色)
    pygame.draw.rect(win, (255, 46, 99), (food[0], food[1], 20, 20))
    
    # 五、分數顯示 (3%)
    txt = font.render(f"Score: {score}", True, (238, 238, 238))
    win.blit(txt, (300 - txt.get_width() // 2, 20))

    # 六、遊戲結束畫面 (3%)
    if game_over:
        go_txt = font.render("GAME OVER", True, (255, 46, 99))
        win.blit(go_txt, (300 - go_txt.get_width() // 2, 250))
        fs_txt = font.render(f"Final Score: {score}", True, (238, 238, 238))
        win.blit(fs_txt, (300 - fs_txt.get_width() // 2, 300))

    pygame.display.flip()
    clock.tick(10)  # 控制遊戲速度 (每秒 10 幀)
