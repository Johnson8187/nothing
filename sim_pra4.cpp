#include <iostream>
#include <cstdio>

int main()
{
	for (int i = 0; i <= 20; i++) {
		unsigned long long factorial = 1;
		for (int j = 1; j <= i; j++) {
			factorial *= j;
		}
		printf_s("%d! = %llu\n", i, factorial);
	}
}
