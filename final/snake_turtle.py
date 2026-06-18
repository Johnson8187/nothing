import turtle
import random

# 遊戲初始設定
snake = [[0, 0], [20, 0], [40, 0]]
direction = [20, 0]
food = [100, 100]
score = 0  # 初始分數

def change_direction(x, y):
    """改變蛇的移動方向，防止直接 180 度回頭"""
    global direction
    if [x, y] != [-direction[0], -direction[1]]:
        direction = [x, y]

def move():
    global food, score
    # 計算新的蛇頭位置
    new_head = [snake[-1][0] + direction[0], snake[-1][1] + direction[1]]
    
    # 碰撞偵測
    if not (-300 < new_head[0] < 300 and -300 < new_head[1] < 300) or new_head in snake:
        print(f"遊戲結束！最終得分: {score}")
        turtle.bye()
        return
        
    snake.append(new_head)
    
    # 吃食物偵測
    if new_head == food:
        score += 1  # 吃到食物分數加 1
        food = [random.randrange(-14, 14) * 20, random.randrange(-14, 14) * 20]
    else:
        snake.pop(0)
        
    # 畫面重繪
    turtle.clear()
    turtle.penup()
    
    # 顯示分數 (在左上角)
    turtle.goto(-280, 260)
    turtle.color("black")
    turtle.write(f"得分: {score}", font=("Arial", 16, "bold"))
    
    # 畫食物
    turtle.goto(food[0], food[1])
    turtle.dot(15, "red")
    
    # 畫蛇
    for segment in snake:
        turtle.goto(segment[0], segment[1])
        turtle.dot(15, "green")
        
    turtle.update()
    turtle.ontimer(move, 150)

# 視窗初始化
turtle.setup(600, 600)
turtle.title("極簡貪吃蛇")
turtle.hideturtle()
turtle.tracer(False)
turtle.listen()

# 鍵盤事件綁定
turtle.onkey(lambda: change_direction(0, 20), "Up")
turtle.onkey(lambda: change_direction(0, -20), "Down")
turtle.onkey(lambda: change_direction(-20, 0), "Left")
turtle.onkey(lambda: change_direction(20, 0), "Right")

# 開始遊戲
move()
turtle.done()
