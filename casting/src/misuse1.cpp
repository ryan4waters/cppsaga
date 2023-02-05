#include <iostream>

using namespace std;

int main()
{
    int *x[2];
    const int a = 10;
    int b = 20;
 
    // Create two const int* pointers, pointing to const variable a and non-const variable b respectively
    const int *aPtr = &a;
    const int *bPtr = &b;

    // The rationality of `const_cast` varies depending on the constness attribute of the original pointed-to object
    x[0] =  const_cast<int*>(aPtr); // bad, non-const pointer element points to a const variable
    x[1] =  const_cast<int*>(bPtr); // good, non-const pointer element points to a non-const variable

    cout << "x[0]:" << *(x[0]) << " x[1]:" << *(x[1]) << endl;
    cout << "a:" << a << " b:" << b <<endl;

    // update
    *(x[0]) = 30; // bad, undefined behavior
    *(x[1]) = 40; // good, original value is non-const

    cout << "x[0]:" << *(x[0]) << " x[1]:" << *(x[1]) << endl;
    cout << "a:" << a << " b:" << b <<endl;
}