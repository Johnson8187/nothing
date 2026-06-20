student_id = int(input("請輸入學號："))
age = int(input("請輸入年齡："))
gentle = input("請輸入性別(男/女)：")
height = int(input("請輸入身高(cm)："))
weight = int(input("請輸入體重(kg)："))
waistline = int(input("請輸入腰圍(cm)："))

BMI= weight/((height/100)**2)
if(gentle=="男"):
    BMR=10*weight+6.25*height-5*age+5
if(gentle=="女"):
    BMR=10*weight+6.25*height-5*age-161

BMI_situation = 0

print("BMI = ",BMI)
if(BMI<18):
    print("BMI狀態：過輕")
    BMI_situation=1
elif(BMI>=18 and BMI<24):
    print("BMI狀態：正常")
elif(BMI>=24 and BMI<27):
    print("BMI狀態：過重")
    BMI_situation=2
else:
    print("BMI狀態：肥胖")
    BMI_situation=2


waistline_situation = 0
if(gentle=="男" and  waistline>=90):
    print("腰圍異常")
    waistline_situation = 1
elif(gentle=="女"and  waistline>=80):
    print("腰圍異常")
    waistline_situation = 1
else:print("正常腰圍")

print("BMR = ",BMR)

if(BMI_situation==0 and waistline_situation==1):print("注意隱性肥胖")
elif(BMI_situation==2):print("建議控制飲食運動")
elif(BMI_situation==1):print("建議增加營養攝取")
else:print("健康狀況良好")



    
