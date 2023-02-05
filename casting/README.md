# How to use `const_cast` correctly

## Summary

A Cast operator is an unary operator which forces one data type to be converted into another data type.

C++ supports four types of casting:

1. Static Cast
2. Dynamic Cast
3. Const Cast
4. Reinterpret Cast

In this article, only cases are used to discuss the correct use of `const_cast` .

Extensive use of `const_cast` can be seen in some C++ projects. There are many misuses in it, and the situation of use is complicated, making it difficult to maintain.

In this article, I list some misuse cases from `const_cast` through some code. It is intended to prevent developers from making mistakes, and unify consensus on `const_cast`. Each misuse point contains one or more cases.

I can't list all the situations, it's just for throwing out a brick to attract a jade, if there are fallacies, please criticize and correct!

## Introduction

In C++,  `const` means that the object or variable it modifies isn't modifiable. `const_cast` can remove this constant protection of variables, so that contents can be modified normally. But this brings some security risks, so it is often not recommended to use it in some coding specifications.

So when can `const_cast` be used? Strictly speaking, `const_cast` is only reasonable if the `const` pointed to content is itself mutable (can be modified).

[References](http://blog.aaronballman.com/2011/06/when-should-you-use- `const_cast` /)

In most cases, the use of `const_cast` often implies design problems, and the rationality of the code needs to be reviewed!

## Misuse 1: A non-const array contains both const and non-const variables

* Security implications: Modifications to certain array elements can lead to data inconsistencies
* Overall suggestion: The constness of the elements in the array should be consistent, that is, the const array only stores const variables, and vice versa
* Case analysis: In this example, the array x contains two int pointers. The code stores two pointer variables pointing to `const int` and `int` for it respectively, and then modifies the contents pointed to by the array elements to see if it will modify the original pointer contents.

```cpp
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
```

The output prints:

![iamge](https://github.com/ryan4waters/cppsaga/blob/main/casting/figures/misuse1.png)

It can be seen that after the update operation, there is an inconsistency between `x[0]` pointing to the integer variable value (30) and a (10), although the pointers are the same (below). The data of `x[1]` and `b` are consistent.

* before update: 
![iamge](https://github.com/ryan4waters/cppsaga/blob/main/casting/figures/before_update.png)
* after update:
![iamge](https://github.com/ryan4waters/cppsaga/blob/main/casting/figures/after_update.png)

This is because when `a` is a constant (const int), when the compiler prints `*x[0]`, it will directly load the constant value 10 into the register. And when `a` is a non-constant (int), when the compiler prints `*x[0]`, it will print the value pointed to by its real address (my vscode cracks that I can't read register to display figure here, update later).

Use the `g++ -S <cpp file name>` command to compare the assembly language when printing `*x[0]` in the two cases of a constant and non-constant (vimdiff).

This causes the inconsistency between the array elements and the original data they point to.

## Misuse 2: Redundant `const_cast` conversions

### Case 1: `const_cast` to the array name

```cpp
int containerItem[MAX];
bool ProcessItem(const int *item);

int* item = const_cast<int*>(containerItem);
bool result = ProcessItem(item);
```

The code has two problems:

1. Although the value of the array name cannot be modified, it is not a constant, so it is unreasonable to use `const_cast` to remove its constant attribute itself
2. The formal parameter type of `ProcessItem` is `const int*`, and the pointer can be directly assigned to the formal parameter without conversion, such as `ProcessItem(containerItem)`.

### Case 2: If the class already defines a const constructor, no explicit conversion is required

```cpp
template<typename T>
class X {
	explicit X(T* a); // non-const constructor
	explicit X(const T* a)； // const constructor
};
typedef X<DataTable> tb;
tb tableA(const_cast<DataTable*>(&anotherTable)); // anotherTable is a constant, call the non-const constructor of class X here
```

In fact, if `const_cast` is not used here, the program will automatically call the const constructor of class `X` without `const_cast` conversion. This is why recommend that a C++ class also need to define a const constructor.

### Case 3: If the assignment object is also const, it is unnecessary to use `const_cast`

```cpp
const int* buf;
buf = const_cast<int*>(item); // actually no need for `const_cast` conversion
```

## Misuse 3: Inappropriate `const_cast` conversions

Misuse 3 is redundant and has low security risks. However, the conversion here may cause security risks

```cpp
int getId(int id)
{
	StructA *a = const_cast<StructA*>(getStruct<StructA>(STRUCT_A_ID)); // The return value of the getStruct function is `const Struct*`
	if(a == nullptr) {
		return INT_MAX;
	}
	// get aid without any modification to a
	return aid;
}
```

In this function, the `const_cast` operation on the return value of the `getStruct` function makes the `a` variable lose constant protection, but the subsequent function does not make any modifications to `a`, so it is a wrong `const_cast` conversion.

## Misuse 4: API‘s formal parameters const consistency problem

Unreasonable API design leads to `const_cast` for every call, which not only makes the code complex and bloated, but also inconsistent const can easily lead to incorrect data modification which is difficult to debug.

The following example excerpts the constant declaration of the formal parameter of `data` in a function call chain:

```
parentSaveNode(void *data)
	-> childSaveNode(void *data)
		-> Changed(const uint8_t *data)
		-> Store(const void *data)
```

The innermost function has const protect to `data`, and in the process of calling the chain, `data` has not been modified the content. Therefore, the functions in the calling chain can do const protect safely, avoiding `const_cast` conversion when calling .

Note: If the call chain API is designed to allow modification of `data`, then the innermost call chain should delete the const. In short, const protection needs to follow the principle of consistency.

Refactoring  tip:

First, add const to the top-level API, and temporarily perform `const_cast` conversion for inner functions that have not yet added const.

```cpp
void parentSaveNode(const void *data) {
	...
	childSaveNode(const_cast<void*>(data));
	...
}
```

Next, modify the API call:

```cpp
void caller() {
	// parentSaveNode(const_cast<void*>data);
	parentSaveNode(void *data);
}
```

Note: During the modification process, it may be found that more function calls need to remove const, and the method of `const_cast` conversion can also be used for temporary processing (similar to piling).

Then use the same two-step method to modify the second layer API. Using this gradual refactoring method in daily work can increase the const consistency in the API call chain and avoid error modification.

[Refactoring](https://martinfowler.com/books/refactoring.html)