
#include <iostream>

int main()
{
	struct student
	{
		char name[20];
		int age;
		double score;
	};
	printf("Enter name：");
	student stu1;
	scanf_s("%s", stu1.name, sizeof(stu1.name));
	printf("Enter age：");
	scanf_s("%d", &stu1.age);
	printf("Enter score：");
	scanf_s("%lf", &stu1.score);
	printf_s("--- Student Info ---\n");
	printf("Name: %s\n", stu1.name);
	printf("Age: %d\n", stu1.age);
	printf("Score: %.2f\n", stu1.score);
}


