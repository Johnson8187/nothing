#include <iostream>
#include <vector>
using namespace std;

// ========== printVector 函式 ==========
void printVector(const vector<int> &data) {
    for (int element : data) {
        cout << element << " ";
    }
    cout << endl;
}

// ========== squareVector 函式樣板 ==========
template <typename T>
void squareVector(vector<T> &data) {
    for (int i = 0; i < data.size(); i++) {
        data[i] = data[i] * data[i];
    }
}

int main() {
    // 建立 vector
    vector<int> v = {1, 2, 3, 4};
    
    // 呼叫 printVector (修改前)
    cout << "Before: ";
    printVector(v);
    
    // 呼叫 squareVector
    squareVector(v);
    
    // 再呼叫 printVector (修改後)
    cout << "After: ";
    printVector(v);
    
    return 0;
}