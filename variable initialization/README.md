# Variable initialization in C++

## Introduce

In software development, the initialization of variables is a topic that does not seem to be so advanced but is always around.

Do all variables need to be initialized? How to efficiently initialize large structures? Faced with a variety of C++ initialization methods, such as aggregate initialization, copy initialization, zero initialization, initialization list, curly braces, parentheses, etc. How should developers make the right choice?

This article discusses these issues in a question-and-answer format.

## Question List

* Q1: Do all variables need to be initialized?
* Q2: How to deal with the performance problem of large data structure initialization?
* Q3: How to choose the correct initialization method?
* Q4: Does struct also need a constructor?


## Q1: Do all variables need to be initialized?

A: The answer is YES!

This conclusion applies to all C++ variable types (class, struct, built-in types, etc.). It is a good programming habit to initialize all variables, which can prevent the program from using some variables with undefined values, which are often a nightmare for debug!

This is also the suggested programming rule in the [C++ Core Guidline](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#Res-always).

If the story ends here, it would be too simplistic. In actual combat, this rule requires us to use it flexibly, and the premise of flexible use is to understand the details of C++ data initialization.

### Detail 1: The C++ standard stipulates that when the program starts, variables of type static and thread_local are initialized with zero

So in theory we don't need to zero-initialize these variables, especially for some large structure variables (from a performance point of view).

But from a practical point of view, if there is no performance bottleneck, from the point of view of readability, it is better to uniformly and explicitly initialize.

```cpp
static int g_var[10];		// zero initialization
thread_local int g_thr[10]; // zero initialization
static int g_num;			// zero initialization
static int var = 10;		// non-zero initialization, requires manual initialization
```

The above conclusions also apply to the static variables declared in those functions.

However, there are two exceptions here:

1. If the initial value of these variables is not zero, then they still need to be explicitly initialized
2. For reference type variables, they must be explicitly initialized

Note: The reason for the zero initialization of the static variable is interpreted as being located in the bss segment in the object file, and the variables in the bss segment will be initialized to zero, which is not rigorous. There is no uniform standard in the industry for the zeroing operation of the bss segment, although most loaders will do so.

### Detail 2: Assign a new value immediately after the variable is initialized, and the compiler can optimize the redundant initialization

With this feature, we can safely initialize variables without worrying about redundant initialization.

To give an example: `number` is assigned a new value immediately after declaration, will there be redundant initialization?

```cpp
int number = 0;
number = GetNumber();
```

This is actually a false proposition. In modern compilers, if a variable is assigned a new value directly after initialization, its initialization operation will be optimized away. That is, the previous example is equivalent to the following code:

```cpp
int number;
number = GetNumber();
```

This optimization applies not only to built-in variables such as int, double, or pointers, but also to arrays.

```cpp
int arr[MAX_USER] = {10}; // compiler won't perform zero initialization, redundant initialization is optimized away
memset(arr, 'a', sizeof(arr));
```

And user-defined objects such as class and struct: For example, in the following piece of code, the constructor of the `Base` class will not actually be called.

```cpp
class Base {
public:
	Base();
	char arr[1024];
};

Base::Base() : arr(){}

void foo()
{
	Base b; // Base::Base() will not be called, redundant initialization is optimized away
	memset(b.arr, 'a', 1024);
	Dummy(b.arr);
}
```

Speaking of this, some people may ask, since the compiler will optimize away redundant initialization, why do we have to manually initialize variables? Isn't this superfluous?

This is because the compiler cannot guarantee that redundant initialization can be removed in any case (compiler optimization must be performed without changing the behavior of the program, so it is conservative).

For example, as the project evolves, the code just now may become the following version:

```cpp
void foo()
{
	Base b; // Call Base::Base() to initialize the array, because the Dummy function may read and write the b.arr array before memset
	Dummy(b.arr); // call Dummy before memset
	memset(b.arr, 'a', 1024);
	Dummy(b.arr);
}
```

If the `arr`  is not initialized in the constructor of the `Base` , then the `Dummy` function may handle undefined values, causing potential bugs.

Summary: For the scenario of assigning a new value after the variable is declared, it is recommended to initialize it at the time of declaration. The compiler can optimize away redundant initialization operations. If the compiler cannot remove redundant operations, it is likely to indicate that there are variable read and write operations between initialization and assignment. At this time, variable initialization is more needed to eliminate potential risks caused by undefined values.

## Q2: How to deal with the performance problem of large data structure initialization?

A: In business code, we often encounter this situation: the cost of data initialization exceeds its data processing cost. However, when performance problems arise due to compliance with the "all variables must be initialized" principle, it still needs to be optimized from the perspective of business logic.

Here are a few ideas:

1. Avoid unnecessary initialization, split the big data structure, and a business process only needs to use the data directly related to itself.
2. Early initialization. Make full use of compile-time initialization features, such as some variables that can be determined at compile time are declared as static, and some variables that remain unchanged during runtime are declared as const, saving runtime assignment costs.
3. Overhead transfer. Transfer the overhead of initialization to data reading.

The following uses an example of printing data in the log to illustrate idea 3:

```cpp
class Rate {
public:
	Rate(uint32_t numerator, uint32_t denomiator) : rate(0) {
		if (denomiator) {
		rate = (double)numerator / (double)denomiator;
		}
	}
	
	~Rate() = default();
	
	void Update(uint32_t numerator, uint32_t denomiator) {
		m_rate = (double)numerator / (double)denomiator;
	}
	
	double Show() const {return m_rate;}
	
private:
	double m_rate;
};
```

`Rate` calculates the ratio using the numerator and denominator when constructing and updating and store it in a double float. In fact, we only need to call Show() when printing the log. In this way, the division operation of the constructor becomes a time-consuming redundant calculation. When the update frequency of the ratio in the system is much higher than the log printing frequency, this consumption is likely to become the bottleneck of the system performance.

At this time we can use some **lazy evaluation** techniques to solve this problem.

```cpp
class Rate {
public:
	Rate(uint32_t numerator, uint32_t denomiator)
	: m_numerator(numerator),
    m_denomiator(denomiator) {
	}
	
	~Rate() = default();
	
	void Update(uint32_t numerator, uint32_t denomiator) {
		m_numerator = numerator;
		m_denomiator = denomiator;
	}
	
	double Show() const {
		return (double)numerator / (double)denomiator;
	}
	
private:
	uint32_t numerator;
	uint32_t denomiator;
};
```

The constructor and update function only save the original data, and the consumed division operation is used when needed (`Show` function).

It should be emphasized that lazy evaluation does not mean no initialization, but transfers the consuming initialization actions to be triggered when they must be executed.

## Q3: How to choose the correct initialization method?

A: It would be okay for a team to maintain a consistent style. Here we focus on the advantages and disadvantages of different methods.

The consistent here mainly involves two aspects:

1. `{}` vs `()`
2. Initializer list vs in-class initialization

### `{}` vs `()`

`{}` is an initialization method introduced by C++, which covers almost all initialization scenarios, so some people also call `{}` "uniform initialization". Almost all scenarios that can be initialized with `()` can be replaced with `{}`.

```cpp
// The following three lines all call the Base()::Base(int) constructor
Base b(1);
Base b{1};
Base b = {1};
```
But `{}` has some features that `()` does not: such as preventing narrow conversion.

```cpp
double x = 0.0;
double y = 1.1;

int sum = (x + y);
int sum = {x + y};
warning: ...
```

But `{}` is not perfect. When it is used in combination with initialization_list, there will be some confusion about overload rules.

![image](https://github.com/ryan4waters/cppsaga/blob/main/variable%20initialization/figures/overload.png)

In terms of practical operation, choose one of `{}` and `()`, and keep the style consistent.

### initializer list vs in-class initialization

C++11 introduces the in-class initialization technology, which can set the initialization value when the class member variable is defined.

```cpp
class Base {
public:
	Base (int i) : i (0) {} // initialization list
};

class Base {
public:
	int i = 0; // in-class initialization
};
```

## Q4: Does struct also need a constructor?

A: Depends on requirements and initialization method

For example, for the following C-style struct, its constructor is generated by the compiler (accurately, it is generated when the program needs it), and this constructor will not do any initialization operations (equivalent to `Item::Item() {}` ).

```cpp
struct Item {
	int i;
	char c;
	char arr[10];
};
```

At this time, different initialization methods will cause completely different results.

```cpp
Item b; // value is undefined
Item b{}; // zero initialization
```

`Item b;` belongs to the default initialization method. In the case of no constructor (such as this example), no operation is performed, so the value of the struct internal is undefined.

`Item b{};` belongs to the initialization method (the equivalent form is `Item b = {}, Item b = Item()`, note that `Item b();` is a function declaration, not a variable definition), when there is no In the case of a constructor (as in this example), the internal data of the struct is zero-initialized.

Therefore, for a C-style struct, if only zero-initialization is required, the above-mentioned value-initialization method can be used instead of manually implementing the constructor, but if it needs to be initialized to a specific non-zero value, a custom constructor is still required.
