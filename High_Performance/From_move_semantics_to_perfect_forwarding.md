# From move semantics to perfect forwarding

## Catalog

1. Basics: left and right values and left and right references
2. std::move
    1. The essence of std::move: static_cast
    2. The purpose of std::move: to prepare for "data ownership transfer + performance optimization (in some cases)"
    3. The trap of std::move: function return value
3. std::forward
4. Summary
5. Practical application of std::move and std::forward

## Summary

Move semantics and perfect forwarding are two important technologies introduced in C++11. The former enhances the program's control over data ownership, and the latter lays the foundation for implementing common template functions. Proficiency in mobile semantics and perfect forwarding is beneficial to the design of safe and high-performance programs.

At present, there are many introductions about these two concepts on the Internet, from various perspectives. Beginners easily fall into the knowledge storm. This article takes the logical concept of move semantics and perfect forwarding as the line, strips away some of the details that are not related to mainstream application scenarios, and explains these two important C++ technologies from a shallow to deep perspective.

## Basics: left and right values and references to left and right values

The two concepts of C++ move semantics and perfect forwarding are implemented through the std::move and std::forward standard library functions. Its header files are all <utility>.

To understand std::move and std::forward, you must first understand lvalues ​​and rvalues.

C++ expressions have two properties: type and value type. The "value type" here refers to lvalue and rvalue. In layman's terms, an expression that can take an address is an lvalue. An expression that cannot take an address is an rvalue.

```cpp
int a = 2;               // a：lvalue

foo(std::string(“a”));   // std::string(“a”)：rvalue

std::string str(“a”);
foo(str);                //str: lvalue

int a = b * c;           // b * c：rvalue

std::string a = "hello"; // "hello": lvalue; because hello is stored in the constant area, the address can be taken
```

Note: Some articles say that the right side of the equal sign is an rvalue, which is not accurate (such as Example 5) and does not conform to the status quo (such as Example 2). In C++, left and right values ​​have long outgrown their use in C. Here we choose whether the address is desirable or not as the criterion for judging. Simple and easy to use.

Rvalues are destroyed at the end of the expression lifetime, so they are generally temporary variables. For example, in Example 4, the result of b*c is of type int and has rvalue properties. After it is assigned to a, the memory will be destroyed.

Finally, when it comes to left and right value references, as the name implies, lvalue references refer to references that have lvalue attribute expressions. An rvalue reference is a reference to an rvalue property expression:

Lvalue reference: "type+&" format, such as int&, std::string&
Rvalue reference: "type+&&" format, such as int&&, std::string&&
So far we have mastered the basic knowledge of left and right values needed to understand std::move and std::forward.

Terminology statement: In the following, we need to distinguish the concept of actual participation parameters. Here, the parameters at the function call point are called actual parameters (argument), and the parameters of the function itself are called formal parameters (parameters).

```cpp
void foo(int a) {}  // a is parameter
foo(a);             // a is parameter
```
When the formal parameters or actual parameters are not emphasized, they are collectively referred to as parameters

## std::move

### The essence of std::move: static_cast

std::move itself does not perform any data movement operations. It has only one operation, which is to convert formal parameters into rvalue references.

Here is a simplified implementation of the std::move function:

```cpp
template<typename T>
typename remove_reference<T>::type&&
 move(T&& param)
{
   using ReturnType = typename remove_reference<T>::type&&;
		 
   return static_cast<ReturnType>(param);
}
```

Here we don't need to understand the details of remove_reference, just treat it as the reference type of the clear parameter, and ensure that the return value of std::move is an rvalue reference.

Since std::move is a static_cast, it is not difficult to understand that it is only a compile-time calculation without any runtime overhead, and the assembly language level is not seen.

So why is a function that does only transformations named move? Where is the real data move operation? In fact, std::move conversion is just an operation at the language implementation level, and its intention is to prepare for data move.

### The purpose of std::move: to prepare for "data ownership transfer + performance optimization (in some cases)"

How do you understand the sentence "std::move is to prepare for the data move operation" above? In fact, you only need to answer the following two questions:

Question 1: What preparation does std::move do?

The following example shows the difference between the copy construction and the move constructor of the C++ class Example in terms of function signatures (assignment functions are similar, omitted here):

```cpp
class Example {
public:
   Example (const Example& other);  // copy construct
   Example (Example&& other);       // move construct
};
```

As you can see, the parameter of the move constructor is an rvalue reference (Example&&). The return value of std::move is an rvalue reference. In this way, according to the function overloading rules, the function converted by std::move will naturally call the move structure to perform the real move operation.

```cpp
// Suppose there is an Example object e
Example(e);            // Call Example copy-construct
Example(std::move(e)); // call Example move-construct
```

This is the meaning behind "std::move function is to prepare for move operation".

Question 2: What exactly does the data move operation refer to?

The purpose of the data move operation is to realize the transfer of data ownership. This is done through the class's move constructor and move assignor.

Here we explain the move operation by comparing the move construction with the implementation of the traditional copy constructor.

For example, there is an int pointer variable var in the Example class, which points to the starting address of a section of memory. The memory pointed to by var is allocated in the default constructor of Example (Example()).

The semantics of the copy constructor are data copies. In this example, it will create a new memory space, and then copy the source memory to the new memory. The result is two memories with identical contents.

```cpp
Example(const Example& other) {
   this->var = new int[other.len];               // Allocate new memory
   memcpy ( var, other.var, sizeof(other.var) ); // Copy memory
}
```

The semantics of the move constructor is the transfer of data ownership. So, in the example above, the move constructor actually just needs to copy the pointer and reset the source pointer to NULL.

```cpp
Example(Example&& other) {
    this->var = other.var;  // Copy pointer only
    other.var = nullptr;    // Null source pointer
}
```

In this way, memory ownership is transferred from the other object to the new Example object. The data after std::move can no longer be used unless it is reassigned.

Some students may ask here, when the source memory is due to the end of the life cycle, the memory will be released, so will there be no pointer hanging problem in the new Example object? This is a great question!

In the above example, the object pointed to by the Example::var variable is the heap (heap) memory, which can only be explicitly released by the developer (such as delete), so the move constructor can only copy the pointer. However, if Example::var points to stack memory, then just copying the pointer will result in a dangling pointer. At this time, the memory data still needs to be copied.

```cpp
  Example e(Example());     
  // The heap memory allocated by the Example() call to the default constructor is not freed.
  // Instead, it continues to be transferred (lifecycle continued) to object e through the move constructor
```

This example also illustrates why encourage everyone to use smart pointers in C++ instead of raw pointers. The former can automatically release the memory at the end of the life cycle, while the latter needs to call delete manually.

Another bonus of the move constructor in this example is a performance boost.

In the Example move constructor, we don't need to do a memory copy, just the pointer itself. However, when the data in Example is not a pointer but an int type, the operation performance of copy and move is actually the same, and both need to copy the int variable.

std::move usage summary:

std::move prepares for the subsequent real move operation. The way is to call the corresponding move constructor (or move assignment function) by converting the formal parameter into an rvalue reference and using the function overloading rules

The real move operation is implemented by the std::move constructor and assignment function of the class. Its purpose is data ownership transfer. Therefore, std::move is generally used in scenarios where data needs to be transferred to new data structures and functions, such as initialization lists and function input parameters.

The move constructor performs better than the copy constructor in some cases, such as containing pointer data

A class without a move constructor/assignment function will fall back to calling a copy constructor/assignment function

unique_ptr can only use std::move for assignment or parameter passing

### The trap of std::move: function return value

Since std::move can transfer data ownership, and the triggered move constructor has better performance sometimes, can we return a local variable of a function through std::move?

```cpp
std::string foo()
{
    std::string str("a");
    return std::move(str); // bad idea!
}
```

It can be used like this, but it will not achieve the purpose of performance optimization.

In a local test, foo() is repeated 10 million times. Use time to grab the total execution time of the program. The "return str" version took 204ms and the "return std::move(str)" version took 359ms!

Why does the use of std::move increase the running time of the program? Because std::move prevents a compiler measure called Return Value Optimization (RVO). RVO avoids copy and move constructor calls by directly creating memory for local variables to be returned in the function return value memory area. But the return value of std::move is a reference, and a reference cannot trigger RVO.

So for those cases where RVO cannot be triggered, can std::move be used? The answer is still no.

In the following example, different branches return different return values, and which branch to execute needs to be determined at runtime, which prevents RVO from being executed at compile time. However, in this case, the compiler automatically optimizes "return str" to "return std::move(str)".

```cpp
std::string test(bool t) {
    if (t) {
       std::string str("a");
       return str; // == std::move(str)
    } else {
       return std::string("b");
    }
}
```

In conclusion, returning local variables, it is not recommended to use std::move to improve performance.

## std::forward

In layman's terms, std::move is "excellent" enough until it encounters template functions.

In the case of non-template functions, the function can be overloaded according to the left and right value attributes of the parameters (as in the following example).

```cpp
std::string str("a");
process(str);            // Call foo(const std::string &)
process(std::move(str)); // Call foo(std::string&&)
```

In the case of template functions, the situation is quite different.

See an example of a variadic template:

```cpp
template <class... Args >
void wrapper(Args&&... args)
{
    // pre-foo args
    process(args...);
}

std::string str("a");
wrapper(str);              // Call process(const std::string&)
wrapper(std::string("a")); // Call process(const std::string&)
```

Regardless of whether the actual parameter in the wrapper is an lvalue or an rvalue attribute, the lvalue attribute version of process will be called!

There are two reasons for this discrepancy:

The formal parameter type Args&& can be either an lvalue reference or an rvalue reference (Args&& is actually a forward reference, which will be elaborated below)

Therefore, the left and right value properties of the actual parameters of the wrapper template function (str and std::string("a") in the above example) are lost in the formal parameters (args in the above example). As a result, when the parameter is passed further, the left and right value attributes of the original parameter cannot be passed on.

At this time, std::forward needs to appear.

```cpp
template <class... Args >
void wrapper(Args&&... args)
{
    // pre-foo args
    process(std::forward<Args>(args)...);
}

std::string str("a");
wrapper(str);              // Call process(const std::string&)
wrapper(std::string("a")); // Call process(std::string&&)
```

In terms of expression, the role of std::forward is to preserve the left and right value attributes of the actual parameter for the lower-level function call.

From the principle of implementation, std::forward can be regarded as a conditional static_cast. When the function argument is an lvalue, std::forward does not perform any conversion. When the function argument is an rvalue, std::forward converts the formal parameter to an rvalue reference.

Finally, let's talk about forward references.

We said at the beginning of the article that the form of "type +&&" is an rvalue reference, such as int&&. And when this type needs to be deduced (such as a template parameter or auto), it is a forward reference. A forward reference can be either an lvalue reference or an rvalue reference (so some people in the early C++ community called it a universe reference).

"Forward reference + variable parameter template + std::forward" can implement a general template function. This is also a typical usage scenario of std::forward.

## Summary

| std::move | std::forward |
| :----- | :---- |
| For Data Ownership Transfer | To preserve the left and right value attributes of the parameter |
| unconditional static_cast | conditional static_cast |
| Does not perform the move operation, but prepares for the move (static_cast is an rvalue reference), and the real move is completed by the move constructor/assignment function | The forwarding operation is not performed, but the left and right value attributes of the actual parameter are kept. The real data forwarding operation is completed by the copy (the actual parameter points to the lvalue reference scenario), move (the actual parameter points to the rvalue reference scenario) construction/assignment function |
| The move constructor/assignment function performs better than the copy/assignment constructor in some scenarios (such as only shallow copy is required) | The template function calling std::forward does not support some parameter types (such as bit fields, etc.) |

## Practical application of std::move and std::forward

### std::move and "Rule of Three/Five/Zero"

As mentioned earlier, std::move is just a static_cast that converts a type property to an rvalue reference. The real data movement is done through the move constructor or move copy assignment function.

```cpp
class Example {
public:
   ~Example();
   Example (const Example& other);      // copy-construct
   Example (Example&& other);           // move-construct
   Example& operator=(Example& other);  // copy-assignment
   Example& operator=(Example&& other); // move-assignment
  }
```

In actual development, many developers don't know whether all the above functions need to be implemented by themselves, or whether they are fully implemented or partially implemented.

Therefore, here are a few well-known but confusing guidelines in C++.

Rule of three: If the user needs to customize any one of the destructor, copy constructor, or copy assignment function of the class, all three functions need to be customized. In other words, either don't customize all three, or customize all of them.

When do users need to implement these functions? One of the common usage scenarios is when there is resource-related data. For example, for a pointer variable, if there is no copy assignment function, then the object will only perform a shallow copy when copying (only the pointer is copied, and the memory data pointed to by the pointer is not copied). Similar examples include system resource variables such as file identifiers, sockets, etc.

Why do these three functions have to be implemented at the same time? Because the management of resources must be properly handled in both destruction and copying, which cannot be done by the functions implicitly generated by the compiler.

Rule of five: If the user defines the three functions in Rule of three and wants to use move semantics, he must customize the move constructor and move copy assignment function at the same time.

Many people misunderstand Rule of Five as that as long as any one of the five functions is customized, the remaining four must be customized. This is not accurate. If the class does not want to use move semantics, it only needs to follow the Rule of Three.

When should you consider using move semantics? A typical scenario is when the copy operation can be simplified to a relatively low-cost move operation (such as a pointer deep copy becomes a shallow copy) (see the discussion in the std::move section).

The first two principles answer when you need to customize the above five functions, while the following Rule of Zero answers when you don't need it.

Rule of Zero: If you can avoid customizing the above 5 functions, avoid customizing.

For example, if the member variables of a class are all POD types (if you do not understand POD, you can simply understand it as a C-style data structure), or the classes of the member variables themselves all conform to the Rule of three/five (example below).

```
struct Example {
public:
    // ... no default operations declared ...
private:
    std::string name;
    std::map<int, int> exampleMap;
};
```

### std::move and STL

The STL in the C++ standard currently supports the std::move operation.

```cpp
std::vector<std::string> vec;
std::string str = "Example";
vec.push_back(std::move(str)); // string str data is moved to vec, cleared
```

3) Typical actual combat scenarios of std::forward

The usage scenario of std::forward is mainly the template function, especially the actual variable parameter template. In daily code development, if the template is not involved, it is generally used less.
