#include <iostream>

int main() {
    auto T = [&](auto x) { return [&](auto y) { return y([&](auto z) {return x(x)(y)(z); }); }; };
    auto X = T(T);
    auto fib = X([](auto f) { return [&](auto n)->int { return n < 2 ? n : f(n - 1) + f(n - 2); }; });
    std::cout << fib(5);
    return 0;
}