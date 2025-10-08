print("階乘表的列印！\n\n請輸入一個正整數:")
n = int(input())
for i in range(1, n + 1):
    fact = 1
    for j in range(1, i + 1):
        fact *= j
    print(f"{i:2d}! = {fact}")
