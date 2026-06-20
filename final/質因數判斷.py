def is_prime(n):
    for i in range(2,n):
        if(n%i==0):
            return False
    else:return True

x = int(input("請輸入一個整數："))
prime=[]

for i in range(2,x+1):
    if is_prime(i)==True:
        prime.append(i)

print("質因數有：",end= " ")
for p in prime:
    print(p,end=" ")
print("\n質數個數：",len(prime))

