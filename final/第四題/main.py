import guess

# 1. 使用者輸入出拳
user = input("請輸入「剪刀」、「石頭」或「布」？")

# 2. 呼叫自訂模組取得電腦出拳
computer = guess.get_choice()
print(f"電腦出 {computer}")

# 3. 判斷並輸出勝負結果
if user == computer:
    print("平手")
elif (user, computer) in [("剪刀", "布"), ("石頭", "剪刀"), ("布", "石頭")]:
    print("你贏了")
else:
    print("你輸了")
