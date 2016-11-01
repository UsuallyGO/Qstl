
#include <iostream>
#include "qstl/qvector.h"

using namespace std;

void showvec(QVector<int>& vec)
{
    size_t s = vec.size();
    cout<<"vec.size():"<<s<<endl;
    s = vec.capacity();
    cout<<"vec.capacity():"<<s<<endl;
    for(size_t i = 0; i < vec.size(); i++)
        cout<<"vec["<<i<<"]:"<<vec[i]<<endl;
}

int main()
{
    QVector<int> myvec(5);

    showvec(myvec);

    for(int i = 0; i < 5; i++)
        myvec[i] = i;

    QVector<int>::iterator iter;
    iter = myvec.begin();
    myvec.insert(iter + 2, 10);
    showvec(myvec);

    myvec.insert(myvec.end(), 100, 11);
    showvec(myvec);

    myvec.resize(2);
    showvec(myvec);

    myvec.clear();
    showvec(myvec);

    myvec.resize(7);
    iter = myvec.begin();
    myvec.insert(iter, 1, 100);
    iter = myvec.begin();
    myvec.insert(iter, 1, 200);
    iter = myvec.begin();
    myvec.insert(iter, 1, 300);

    iter = myvec.begin();
    myvec.insert(iter+1, 13, 27);
    showvec(myvec);
    for(iter = myvec.begin(); iter != myvec.end(); iter++)
        cout<<"vec:"<<*iter<<endl;

    myvec.push_back(18);
    showvec(myvec);
    myvec.clear();
    myvec.push_back(188);
    showvec(myvec);

    return 0;
}
