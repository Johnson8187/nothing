n = int(input("請輸入一個正整數:"))
for i in range(1, n+1):
    if i == n:
        # 最後一行輸出所有星號
        print("*" * (2 * n))
    else:
        # 計算中間的空格數量
        space = " " * (2 * (n - i))
        print("*" * i + space + "*" * i)
