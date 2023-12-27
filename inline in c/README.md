# inline in C

## Intro

I will discuss the following issues in this article:

1. Why is `inline` almost always used to modify static functions?
2. Is the inline function always be inlined?
3. Why should we be particularly careful with inlining non-static functions?

## Why is `inline` almost always used to modify static functions?

`inline` will only affect the compilation behavior of the function within "translation uint" (which can be simply understood as a .c file). As long as it exceeds this range, the inline attribute has no effect. Because the compiler compiles each .c file separately during compilation, and only looks at this one file during the compilation process (Even for included header files, the preprocessor will copy and "paste" the contents into the corresponding include file before compilation begins.). So if a function is defined in another .c file, the compiler has no way of knowing the implementation of this function (`LTO` is not discussed here), and it cannot expand it inline. In this case the compiler can only generate a call instruction to the external function, and the linker will eventually link to the non-inline version of the external function.

So in most cases, adding `inline` to a global function does not make much sense. Because usually when we inline a function, we hope that it can be inlined in all places where it is called. As a global function exposed to the outside world, it can only be inlined by the .c file in which it is defined. In most cases, it does not meet our expectations (Of course, there are very few scenarios where this is really necessary, so there are various tricky non-static inline situations that will be mentioned later).

Therefore, for a function that needs to be exposed to the outside, if we want it to be inlined by external callers, the general way to write it is to put its definition in the header file, and then include it by the caller. In this way, after the preprocessor expands the header file, it is equivalent to copying the code of the function in the caller's .c file, so that the compiler knows how to inline it. Since the function names of these function code copies expanded by the preprocessor are all the same, in order to avoid global symbol conflicts, static should be added to limit their scope. So we will see some small global functions defined as `static inline` and placed in the header file instead of functional macros.

## Is the inline function always be inlined?

The `inline` keyword merely **suggests** that the compiler do inline expansion processing, not forcing it. In fact, a compiler that completely ignores `inline` and does not support function inlining is also fully compliant with the C standard.

For a `static inline` function, if the compiler decides not to inline it, it behaves like a normal static function - the compiler will generate an internal non-inline function that can be referenced by all callers in the scope. The compiler may also generate a non-inline version while inlining: for example, the function is taken from the address (for example: referenced by a function pointer as a callback function), or there is a recursive call. In these cases where it cannot be inlined the compiler may choose to refer to its non-inlined version and expand it inline where it is otherwise inlineable.

Generally speaking, when compiler optimization is turned off, the compiler will disable inlining, which makes debugging easier. Under different optimization levels, the compiler may adopt different inlining strategies. In addition, if an inline function is analyzed by the compiler as not suitable for inlining (for example, the compiler believes that the performance gain after inlining is not enough to offset the code expansion caused by inlining and other problems), the compiler will not implement inlining. You can add `-Winline` to have the compiler generate a warning in this case.

## Why should we be particularly careful with inlining non-static functions?

Let’s take you through the interesting behaviors of several `inlines` besides `static inline` below - first prepare the following files:

```c
#a.c
#include <stdio.h>

extern inline void print(void)
{
    printf("extern inline a\n");
}

void a(void)
{
	print();
}
```

```c
#b.c
#include <stdio.h>

inline void print(void)
{
    printf("inline b\n");
}

void b(void)
{
	print();
}
```

```c
#main.c

extern void a(void);
extern void b(void);
extern void print(void);

int main(void)
{
    a();
    b();
    print();
    return 0;
}
```

Compile with GNU89 first:

```bash
gcc -std=gnu89 -O2 a.c b.c main.c
```

Since C89 does not support `inline`, what actually takes effect at this time is GCC’s traditional inline behavior:

```bash
$ ./a.out
extern inline a
inline b
inline b
```

You can see that the actual generated externally visible version of the `print` function is the version in b.c. The `extern inline` in a.c is inlined in its file, replacing the external function of the same name.

Let’s look at the situation after C99 (GNU99/C99):

```bash
gcc -std=gnu11 -O2 a.c b.c main.c
```

Since C99, `inline` has officially become one of the keywords of the C language, because GCC has also aligned the default `inline` behavior to the standard:

```bash
$ ./a.out
extern inline a
inline b
extern inline a
```

This time the external version has become the one in a.c. Therefore, the effects of `extern inline` and `inline` of GNU89 and C99 are exactly opposite:

* When GNU89's `inline` is used alone to modify the definition of a function, it may be inlined in the file where the function is located, and at the same time generate an externally visible non-inline version; while C99's `inline` is the opposite - it must A non-inline version of the corresponding function will not be generated (provided that the function is not declared as `extern`)
* When GNU89's `extern inline` is used to modify the definition of a function, its behavior is consistent with the previous C99 modification using `inline` alone. It will not generate a non-inline version. In C99, when the definition of a function that has been declared as `extern` is modified by `inline`, it will definitely generate an externally visible version (the behavior is consistent with GNU89's `inline`).

But pay attention to the previous wording: **may** be inlined - as emphasized before, `inline` is only an inlining suggestion to the compiler, but whether to do inlining or not is completely decided by the compiler. So what is the result of compilation and execution when the compiler decides not to inline? Here we trigger this behavior by turning off compilation optimization:

* GNU89

```bash
gcc -std=gnu89 a.c b.c main.c
```

```bash
$ ./a.out
inline b
inline b
inline b
```

Since the compiler in a.c decided not to inline, it instead called the externally visible version of `print`, which is the implementation in b.c.

* GNU99/C99以上

```bash
gcc -std=gnu11 a.c b.c main.c
```

```bash
$ ./a.out
extern inline a
extern inline a
extern inline a
```

Similarly, after abandoning inlining, all call points use the externally visible version in a.c.

So except for the most harmless `static inline`, `inline` and `extern inline` have opposite effects between GNU89 and GNU99/11, and may also behave differently under different inlining decisions. It is difficult to expect the average developer to maintain a clear understanding of this mess. In most cases, the non-static inline seen in the code is more likely to be the result of blindness. So unless you are a C master, it is not recommended to use any other type `inline` in your code except `static inline`.