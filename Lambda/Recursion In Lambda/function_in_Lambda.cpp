#include <iostream>
#include <functional>
int main(int argc, char* argv[])
{
	std::function<int(int)> fib = [&fib](int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); };
	std::cout << fib(5);
	return 0;
}