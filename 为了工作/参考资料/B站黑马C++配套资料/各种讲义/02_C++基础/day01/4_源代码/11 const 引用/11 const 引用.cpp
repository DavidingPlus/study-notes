#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;


//const 修饰引用，一般跟const修饰指针的用途是一样的。都是作为函数参数，
//保证该参数是输入参数，是只读的，在函数内部该表不了外部的值
void printB(const int &re)
{
	cout << "re = " << re << endl;
}

int main(void)
{
	const int a = 10; //1，如果想对一个常量进行引用， 必须是一个const引用。

	const int &re = a;

	int b = 20;
	const int &re2 = b; //2 相反如果一个普通变量，用一个const引用接收是可以的。

	cout << "b = " << b << endl;
	cout << "re2 =" << re2 << endl;
	//re2 = 300;
	b = 30;
	cout << "b = " << b << endl;
	cout << "re2 =" << re2 << endl; 

	printB(b);
	printB(a);


	const int &re3 = 10;

	//const int &re3 = 10; re3 = &10, 编译器临时开辟一个变量temp = 10；
	//const int &re3 = temp

	const int *p = &re3;

	*((int*)p) = 30; //temp = 30

	cout << "*p = " << *p << endl;
	cout << "re3 = " << re3 << endl;


	return 0;
}

