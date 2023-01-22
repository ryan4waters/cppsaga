# const / static keywords in C++：

## static

* Member variables
  1. Static member variables belong to the class, not to any specific object, and do not occupy the memory space of the object. It can be accessed in the form of `A::n`, or through objects (although it does not belong to a specific object, it is shared by all objects)
  2. The initialization of static member variables must be initialized outside the class, and the type and class to which it belongs must be specified, no need to write the keyword `static` there:

```c++
class A
{
public:
    static int x;
};

int A::x=4;
```

* member functions
  Static member functions can only access static member variables or static member functions, and cannot access non-static member variables and non-static member functions

## const

* Member variables
  Must be initialized in the constructor initialization list. There are two reasons for this:
  
  1. The member variables of the class cannot be initialized in the declaration, for example:

```c++
class A
{
public:
    int x=0; // illegal
};
```

  2. The write operation cannot be performed in a member function, because `const` has already indicated that it cannot be changed

* member functions
   The const member functions can access all member variables, but cannot change the value of any data member of the **object**, but can change the value of static members (static members belong to the class, not to specific objects)

* object
   `const object` can only call const member functions, and can only change static member variables


## static const

In terms of modification: `static const` has both `static` and `const` properties

In terms of writing: `static const` is equal to `const static`

The initialization is the same as static members, which should be operate outside the class, but you need to add const at the beginning

```c++
#include <iostream>
#include <string>

using namespace std;

class Tom
{
public:
    Tom() : p_name("tombrady"), p_age(44), p_number(12){}
    string p_name;
    int p_age;
    int p_number;
};

class Bucs
{
public:
    Bucs() : t_dream("championship"){}
    Bucs(int number, int* age) : t_dream("championship"), p_number(number), p_age(age) {}
    ~Bucs() {
        if (p_age != nullptr) {
            delete p_age;
            p_age = nullptr;
        }
    }

    static void teamnameshorten() {
        t_name = "Bucs";
    }
    static void teamnamecomplete() { // Static member function could write on Static member variable or Static member function
        teamnameshorten();
        t_name = "Tampa Bay " + t_name;
    }

    void teamchampswin() const { // Const function could read all member variable but only could write on Static member variable. If place "const" in front of void, constant object couldn't use it.
        t_champs++;
    }

    int p_number;
    int* p_age;

    static string t_name;
    static int t_champs;
    const string t_dream;
    const static string t_loc;
    static const int t_pnum;
};

// static member variable and constant static member variable initialization
string Bucs::t_name = "Tampa Bay";
int Bucs::t_champs = 2;
const string Bucs::t_loc = "tampa";
const int Bucs::t_pnum = 54;

void test(void) {
    Tom tombrady;
    Bucs p12(tombrady.p_number, &tombrady.p_age);
    cout << Bucs::t_name << endl;
    p12.teamnamecomplete();
    cout << p12.t_name << endl;
    
    cout << p12.t_dream << endl;

    p12.teamchampswin();
    cout << p12.t_champs << endl;

    const Bucs boss;
    cout << boss.t_dream << endl;
    boss.teamchampswin();
    cout << Bucs::t_champs << endl;

    cout << Bucs::t_loc << endl;
    cout << Bucs::t_pnum << endl;
}

int main() {
    test();

    system ("pasue");
}
```

The result of print：[click here](https://godbolt.org/z/5Wb17rzGY).