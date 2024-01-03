#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Base1
{
public:
	Base1()
	{
		m_A = 10;
	}
public:
	int m_A;
};

class Base2
{
public:
	Base2()
	{
		m_A = 20;
	}
public:
	int m_A;
};

//多继承
class Son :public Base1, public Base2
{
public:

	int m_C;
	int m_D;
};

//多继承中很容易引发二义性
void test01()
{
	cout << sizeof(Son) << endl;

	Son s1;
	//s1.m_A; //二义性

	cout << s1.Base1::m_A << endl; 
	cout << s1.Base2::m_A << endl;
}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}