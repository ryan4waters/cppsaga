#include <iostream>

using namespace std;

class Test
{
private:
    int* age;
public:
    Test(int x) {
        this->age=new int (x);
        cout<<"constructor with parameters"<<endl;
    }

    ~Test() {
        if (this->age != nullptr) {
            delete this->age;
            this->age=nullptr;
        }
        cout<<"destructor"<<endl;
    }

    Test(const Test &ttmp) {
        this->age=ttmp.age;
        cout<<"shallow copy"<<endl;
    }

    //Test (const Test& ttt) {
    //    this->age=new int (*ttt.age);
    //    cout<<"deep copy"<<endl;
    //}

    int getage(void) {
        return *(this->age);
    }

    void printadd() {
        cout<<this->age<<endl;
    }
};

void mytest(void) {
    Test t1(99);
    cout<<t1.getage()<<endl;
    t1.printadd();
    Test t2(t1);
    cout<<t2.getage()<<endl;
    t2.printadd();
}

int main() {
    mytest();
    system ("pause");
}
