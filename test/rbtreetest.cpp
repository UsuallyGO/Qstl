
#include <iostream>
#include "qstl/qtree.h"

using namespace std;

int main()
{
    int i;
	_rb_tree<int, int> tree;
	int nums[] = {12, 1, 9, 2, 0, 11, 7, 19, 4, 15, 18, 5, 14, 13, 10, 16, 6, 3, 8, 17};
    for(i = 0; i < sizeof(nums)/sizeof(int); i++){
		cout<<"insert:"<<nums[i]<<endl;
        tree.insert(nums[i]);
        _rb_tree<int, int>::const_iterator iter;
		if(tree.rb_verify()) cout<<"tree is legal"<<endl;
		else cout<<"bad tree"<<endl;
        for(iter = tree.begin(); iter != tree.end(); iter++)
            cout<<"*iter:"<<*iter<<endl;
        cout<<"********"<<endl;
    }

	for(i = 0; i < sizeof(nums)/sizeof(int); i++){
		cout<<"earse:"<<nums[i]<<endl;
        tree.erase(nums[i]);
		_rb_tree<int, int>::iterator iter;
		if(tree.rb_verify()) cout<<"tree is legal"<<endl;
		else cout<<"bad tree"<<endl;
		for(iter = tree.begin(); iter != tree.end(); iter++)
			cout<<"*iter:"<<*iter<<endl;
		cout<<"********"<<endl;
	}

	return 0;
}
