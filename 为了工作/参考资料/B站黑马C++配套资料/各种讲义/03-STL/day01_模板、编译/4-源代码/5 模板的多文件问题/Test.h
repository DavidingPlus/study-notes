#pragma once
#include<iostream>
using namespace std;
template<class T>class Test;
template < class T>void show2(Test<T>&another);

template <class T>
class Test
{
public:
	Test(T age);
	void show();
	
	friend void show2(Test<T>&another);
	~Test();
private :
	T age;
};

