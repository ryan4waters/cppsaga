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
