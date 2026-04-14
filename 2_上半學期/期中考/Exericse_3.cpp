#include <iostream>
#include <vector>
using namespace std;

void printVector(const vector<int> &data) {
	for (size_t i = 0; i < data.size(); i++) {
		cout << data[i] << " ";
	}
}

template<typename T>
void squareVector(vector<T>& data) {
	for (size_t i = 0; i < data.size(); i++) {
		data[i] = data[i] * data[i];
	}
}

int main()
{
	vector<int> v = { 1,2,3,4 };
	cout << "Before：";
	printVector(v);
	squareVector(v);
	cout << "\nAfter：";
	printVector(v);
}
