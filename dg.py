from sympy.logic.boolalg import SOPform
from sympy import symbols

# 1. 定義變數 (4個變數可以表示 0-15 的數字)
A, B, C, D = symbols('A B C D')

# 2. 定義 Minterms 列表
minterms = [3, 5, 6, 7, 9, 10, 11, 12, 13, 14]

# 3. 使用 SOPform 進行化簡 (Sum of Products)
# 第一個參數是變數清單，第二個是 minterms
simplified = SOPform([A, B, C, D], minterms)

print("--- Minterms 化簡結果 ---")
print(f"輸入編號: {minterms}")
print(f"最簡化邏輯式: {simplified}")