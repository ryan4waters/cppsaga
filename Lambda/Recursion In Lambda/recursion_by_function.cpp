#include <iostream>
#include <functional>
int main(int argc, char* argv[])
{
	std::function<int(int, int, int, int)> fib = [&fib](int n, int i = 0, int num1 = 0, int num2 = 1) {
        if (i >= n) return num1;
        else return fib(n, i + 1, num2, num1 + num2);
    };
	std::cout << fib(5);
	return 0;
}