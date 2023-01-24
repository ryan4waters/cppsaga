# Recursion In Lambda

## Introduce
As we all know, Lambda expressions(sames like closures) have appeared since C++11, making the implementation of C++ functional programming easier. This article would not discuss the historical and engineering significance of lambda expressions too much, but just introduces how to call lambda expressions recursively. For the basic usage of lambda expressions, please go to [cppreference ](https://en.cppreference.com/w/cpp/language/lambda) or related blogs. This article assumes that readers have the most basic understanding of lambda syntax.

What you need to read this article:

* Very basic knowledge of C++ syntax
* Very basic knowledge of C++ Lambda expression syntax
* Very basic knowledge of recursion
* Very basic knowledge of Engliash

## Let's roll in

We all know that if you try to call recursively in the function body directly like a normal function, there will be a syntax error, and neither reference capture nor value capture can be used (I won’t go into details here). So we need something else to implement recursive calls.

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/directly_call_in_Lambda.png)

## std::function in C++11
Taking the Fibonacci sequence as an example, we can use lambda expressions to construct a std::function object as blow:

```c++
#include <iostream>
#include <functional>
int main(int argc, char* argv[])
{
	std::function<int(int)> fib = [&fib](int n) { return n < 2 ? n : fib(n - 1) + fib(n - 2); };
	std::cout << fib(5);
	return 0;
}
```

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/function_in_Lambda.png)

But obviously, this method is not so elegant from the point of view of the declaration form. From the point of view of the writing form, the function signature written by the lambda on the right has to be copied again on the left, which is too cumbersome and ugly. In addition, using closures to initialize std::function objects does not essentially solve the problem of lambda recursive calls. It just avoids this problem, but introduces many new problems. It is not a zero-overhead abstraction. In addition, he still has some functional defects. For example, we try to implement a Fibonacci sequence of tail recursive calls:

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/recursion_by_function.png)

Obviously, for lambda expressions with default parameters, std::function cannot realize all its features.

## Based on Y Combinator C++14
If you guys have basic lambda calculus foundation, you would have some concepts about "Y fixed point combinator", which is used to solve the recursive call problem of anonymous functions.

### Currying
The specific theory is not described here, just look at the code directly. The relatively pure Funtional Programming writing method is as follows. Since we need to currying and pass lambda expressions, the generic lambda from C++14 is used here, that is, it is supported in lambda Use `auto` in the argument list of an expression:

```c++
#include <iostream>

int main() {
    auto T = [&](auto x) { return [&](auto y) { return y([&](auto z) {return x(x)(y)(z); }); }; };
    auto X = T(T);
    auto fib = X([](auto f) { return [&](auto n)->int { return n < 2 ? n : f(n - 1) + f(n - 2); }; });
    std::cout << fib(5);
    return 0;
}
```

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/currying.png)

### Directly input (commonly used for LeetCode)

This looks complex, and we can simplify it like this (as far as my personal habits are concerned, this is the form I use when having fun in LeetCode):

```c++
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
```

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/simplified_pass.png)

### Pack

Different from Currying to construct a constructor, here we directly choose to accept a lambda expression as a parameter call. The disadvantage is that we need to pass it in every time we call it, which is troublesome, so we directly use another lambda or std: :bind packs it up:

```cpp
#include <iostream>
int main(int argc, char* argv[])
{
	auto f = [](auto&& self, int n, int i = 0, int num1 = 0, int num2 = 1) {
		if (i >= n) return num1;
		else return self(self, n, i + 1, num2, num1 + num2);
	};
	auto fib = [&f](int n) { return f(f, n); };
	std::cout << fib(5);
	return 0;
}
```

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/pack.png)

But this is actually a bit bad, that is, we only need one `fib`, but there is an extra `f`, which pollutes the namespace, so how to solve it? It is easier to think of directly defining it inside the function body of `fib`:

```c++
#include <iostream>
int main(int argc, char* argv[])
{
	auto fib = [](int n) {
		auto f = [](auto&& self, int n, int i = 0, int num1 = 0, int num2 = 1) {
			if (i >= n) return num1;
			else return self(self, n, i + 1, num2, num1 + num2); 
		};
		return f(f, n); 
	};
	std::cout << fib(5);
	return 0;
}
```

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/f_inside_fib.png)

But it is obvious that every time we call `fib`, we have to re-initialize `f`. If the compiler optimization is not considered, there will be a performance overhead which is plain to see, so here we can use the same initializer from C++14 capture list, so that `f` will only be initialized when `fib` is initialized:

```c++
#include <iostream>
int main(int argc, char* argv[])
{
	auto fib = [
		f = [](auto&& self, int n, int i = 0, int num1 = 0, int num2 = 1) {
				if (i >= n) return num1;
				else return self(self, n, i + 1, num2, num1 + num2);
		}](int n) { return f(f, n); };;
	std::cout << fib(5);
	return 0;
}
```

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/f_in_capture_list.png)

### About Return Type infer

In the end, it needs to be pointed out that I noticed that I was directly returning a ternary operator at the beginning, but here I used the `if-else` statement to return. This is to lead to a problem, that is, the automatically inferred return value problem.
We changed the code to this, and found that a syntax error occurred:

```c++
#include <iostream>
int main(int argc, char* argv[])
{
	auto fib = [
		f = [](auto&& self, int n, int i = 0, int num1 = 0, int num2 = 1) {
				return i >= n ? num1 : self(self, n, i + 1, num2, num1 + num2);
		}](int n) { return f(f, n); };
	std::cout << fib(5);
	return 0;
}
```

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/auto_trailing_return_type.png)

> cannot deduce the return type of function "lambda [](auto &&self, int n, int i = 0, int num1 = 0, int num2 = 1)->auto [with <auto-1>=const lambda [](auto &&self, int n, int i = 0, int num1 = 0, int num2 = 1)->auto &]" (declared at line 5); it has not been defined
>
> detected during instantiation of function "lambda [](auto &&self, int n, int i = 0, int num1 = 0, int num2 = 1)->auto [with <auto-1>=const lambda [](auto &&self, int n, int i = 0, int num1 = 0, int num2 = 1)->auto &]" at line 7

As you can see from reading the error report, we tried to call `self`, but we don't know the return type of `self`. Regarding the reason for this error, the popular description is that the function is called before the return value of the function is known. After figuring out the reason, it is very simple to solve. One way is to return a value before the call as above, so that the compiler can infer the return value in advance, and the other is to use the trailing return type , which is also more general:

```c++
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
```

![image](https://github.com/ryan4waters/cppsaga/blob/main/Lambda/Recursion%20In%20Lambda/figures/trailing_return_type.png)

## Use Deducing this to realize lambda recursion in C++23

In fact, as early as 2017, the proposal [P0839R0](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0839r0.html) made an effort to simplify lambda recursion which tried to do this:

```c++
#include <iostream>
int main(int argc, char* argv[])
{
	auto fib = [] self(int n) {
		if (n < 2) return n;
		return self(n - 1) + self(n - 2);
	};
	std::cout << fib(5);
	return 0;
}
```

It's almost a name for lambda, but it's not implemented...

C++23 has such a proposal: [P0847R7](https://open-std.org/JTC1/SC22/WG21/docs/papers/2021/p0847r7.html#recursive-lambdas), this proposal is not actually prepared for lambda, but lambda recursion can just be used. You can check the details by yourself, here is the usage:

```c++
#include <iostream>
int main(int argc, char* argv[])
{
	auto fib = [](this auto&& self, int n, int i = 0, int num1 = 0, int num2 = 1)->int {
		return i >= n ? num1 : self(n, i + 1, num2, num1 + num2);
	};
	std::cout << fib(5);
	return 0;
}
```

It can be seen that the basic syntax is similar to the form of the Y fixed-point combinator above, and the first parameter also accepts a closure object, but the calling form is more beautiful.

In this way, we can directly make the stack overflow in one line.

```c++
int main(int argc, char* argv[])
{
	[](this auto f)->void { f(); }();
	return 0;
}
```

Unfortunately, LeetCode has not supported it so far...