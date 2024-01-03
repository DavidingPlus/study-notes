#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

//函数的返回值， 函数形参列表（参数的个数，参数类型，参数顺序）
//函数重载  函数名相同， 参数列表不同
//函数返回值并不是构成函数重载的条件
int func(int a, int b) 
{
	cout << "func1" << endl;
	return 0;
}

//如果要是函数重载话，不要写默认参数，为了避免调用出现函数冲突
char func(int a, int b, int)
{
	cout << "func2" << endl;
	return 0;
}

#if 1
int func(int a, char *str)
{
	cout << "func3" << endl;
	return 0;
}
#endif

void print1(int a)
{
	cout << "print 1" << endl;
	cout << "a = " << a << endl;
}

void print1(double b)
{
	cout << "print 2" << endl;
	cout << "b = " << b << endl;
}
void print1(char ch)
{
	cout << "print 3" << endl;
	cout << "ch =" << ch << endl;
}

int main(void)
{
	//func(10, 20);

	func(10, "abc");

	print1(10);

	print1(19.00);

	print1(3.1f);

	print1('a');//char->int

	//print1("itcast");
	//1 如果能够严格匹配调用完全匹配的
	//2 如果没有完全匹配，调用隐士转换
	//3 都匹配不到，调用失败。

	return 0;
}