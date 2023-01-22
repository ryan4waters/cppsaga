#include <iostream>
#include <string>

using namespace std;

class Team
{
public:
    Team(string tstring,int tchamp){
        t_name=tstring;
        t_champ=tchamp;
        cout<<"Team constructor"<<endl;
    }
    ~Team() {
        cout<<"Team destructor"<<endl;
    }

    string t_name;
    int t_champ;
};

class Player
{
public:
    Player(string pename,string phname):p_name(pename),t(phname,2) {
        cout<<"Player consructor"<<endl;
    }

    ~Player() {
        cout<<"Player desructor"<<endl;
    }

    string p_name;
    Team t;
};

void test(void) {
    Player tom("brady","Bucs");
    cout<<tom.p_name<<endl;
    cout<<tom.t.t_name<<endl;
}
int main() {
    test();

    system ("pause");
}
