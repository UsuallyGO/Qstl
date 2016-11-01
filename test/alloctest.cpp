
#include <iostream>

#include "qstl/qalloc.h"

using namespace std;

class A{
public:
    A(){ cout<<"In A constructor"<<endl; }
    ~A() { cout<<"In A deconstructor"<<endl; }

    void Atestf1() { cout<<"In Atestf1()"<<endl; }
};
int main()
{
    int *i = QSimple_alloc<int, 0>::allocate();
    *i = 100;
    cout<<"i:"<<*i<<endl;

    A *a = QSimple_alloc<A, 0>::allocate();
    new(a) A();
    a->Atestf1();

    cout<<"N allocate"<<endl;
    A *array = QSimple_alloc<A, 0>::allocate(5);
    for(int i = 0; i < 5; i++)
    {
        new(&array[i]) A();
        array[i].Atestf1();
    }

    cout<<"QAdvanced_alloc<false, 0>"<<endl;
    A *a1 = (A*)QAdvanced_alloc<false, 0>::allocate(78);
    new(a1) A();
    a1->Atestf1();

    return 0;
}