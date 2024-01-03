#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;


template<class T>
class Person{
public:
	Person(){
		mAge = 0;
	}
public:
	T mAge;
};

//为什么？
//类区定义对象，这个对象是不是需要编译分配内存
//
class SubPerson : public Person<int>{};


int main(void){




	
	return 0;
}