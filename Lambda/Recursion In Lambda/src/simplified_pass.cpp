#include <iostream>
int main(int argc, char* argv[])
{
	auto fib = [](auto&& self, int n, int i = 0, int num1 = 0, int num2 = 1) {
		if (i >= n) return num1;
		else return self(self, n, i + 1, num2, num1 + num2);
	};
	std::cout << fib(fib, 5);
	return 0;
}