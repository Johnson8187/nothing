#include <iostream>
#include <vector>

using namespace std;

void printVector(const vector<int>& data)
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (i > 0)
		{
			cout << ' ';
		}
		cout << data[i];
	}
	cout << endl;
}

template <typename T>
void squareVector(vector<T>& data)
{
	for (T& value : data)
	{
		value *= value;
	}
}

int main()
{
	vector<int> v = {1, 2, 3, 4};

	cout << "Before: ";
	printVector(v);

	squareVector(v);

	cout << "After: ";
	printVector(v);

	return 0;
}
