#include <iostream>
int main(int argc, char* argv[])
{
	auto fib = [
		f = [](auto&& self, int n, int i = 0, int num1 = 0, int num2 = 1)->int {
			return i >= n ? num1 : self(self, n, i + 1, num2, num1 + num2);
		}](int n) { return f(f, n); };
	std::cout << fib(5);
	return 0;
}