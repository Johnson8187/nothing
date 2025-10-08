print("請輸入一個正整數:")
n = int(input())
for i in range(1, n+1):
    # 計算空格數量 (n-i個空格)
    spaces = " " * (n - i)
    
    # 建立數字字串，從 (n-i+1) 到 n
    numbers = ""
    for j in range(n - i + 1, n + 1):
        numbers += str(j)
    
    # 輸出空格 + 數字
    print(spaces + numbers)