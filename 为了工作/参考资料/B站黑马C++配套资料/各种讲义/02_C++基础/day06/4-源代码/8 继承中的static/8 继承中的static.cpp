#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class A
{
public:
	static int a;
private:

};

class B :public A
{
public:
private:
};

int A::a = 100;//静态成员变量 初始化

int main(void)
{
	A a1;
	A a2;

	cout << a1.a << endl;
	cout << a2.a << endl;

	A::a = 300;

	cout << a1.a << endl;
	cout << a2.a << endl;

	B b1;
	B b2;
	A::a = 400;

	cout << "------" << endl;
	cout << b1.a << endl;
	cout << b2.a << endl;
	cout << a1.a << endl;
	cout << a2.a << endl;

	return 0;
}