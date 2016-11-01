
#include <iostream>

#include "qstl/qlist.h"

using namespace std;

void showlist(QList<int>& ql)
{
    cout<<"list.size():"<<ql.size()<<endl;
    cout<<"list.front():"<<ql.front()<<endl;
    cout<<"list.back():"<<ql.back()<<endl;
    QList<int>::iterator iter;
    for(iter = ql.begin(); iter != ql.end(); iter++)
        cout<<"value:"<<*iter<<endl;
}

int main()
{
    QList<int> ql;

    ql.insert(ql.begin(), 10);
    showlist(ql);

    ql.insert(ql.begin(), 20);
    ql.insert(ql.begin(), 30);
    ql.insert(ql.end(), 50);
    showlist(ql);

    return 0;
}
