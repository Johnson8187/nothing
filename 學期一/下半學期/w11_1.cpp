#include <iostream>

int add(int a, int b)
{
	return a + b;
}
int sub(int a, int b)
{
	return a - b;
}

int muiltiply(int a, int b)
{
	return a * b;
}

int main()
{
	int (*op[3])(int, int) = { add, sub, muiltiply };
	int choice;
	printf_s("Enter -> 0=add, 1=subtract, 2=multiply, other=exit：\n");
	scanf_s("%d", &choice);

	while (choice >= 0 && choice < 3) {
		int a, b;
		printf_s("Enter two integers:\n");
		scanf_s("%d%d", &a, &b);
		int result = op[choice](a, b);
		printf_s("Result = %d\n", result);
		printf_s("Enter -> 0=add, 1=subtract, 2=multiply, other=exit：\n");
		scanf_s("%d", &choice);
	}
	printf_s("Exit.");
	return 0;
}
