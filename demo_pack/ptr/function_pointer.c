#include <stdio.h>
#include <stdlib.h>

typedef int (*Fun_Ptr)(int a, int b);

int func0(int a, int b)
{
    return a + b;
}

int func1(int a, int b)
{
    return a - b;
}

int func2(int a, int b)
{
    return a * b;
}

int main()
{
    int a = 1, b = 2;
    Fun_Ptr myFunPtr[] = {func0, func1, func2};
    for (int i = 0; i < sizeof(myFunPtr) / sizeof(Fun_Ptr); ++i) {
        cout << myFunPtr[i](a, b) << " ";
    }

    return 0;
}
