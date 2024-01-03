#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Base
{
public:
	Base()
	{
		m_A = 100;
	}

	void fun()
	{
		cout << "Base func调用" << endl;
	}
	void fun(int a)
	{
		cout << "Base func (int a)调用" << endl;
	}


	int m_A;
};

class Son :public Base
{
public:
	Son()
	{
		m_A = 200;
	}

	void fun()
	{
		cout << "Son func调用" << endl;
	}

	int m_A;
};

void test01()
{
	Son s1;
	cout << s1.m_A << endl;
	//想调用 父类中 的m_A

	cout << s1.Base::m_A << endl;

	s1.fun();

	//调用父类的func
	s1.Base::fun(10);

}

//如果子类和父类拥有同名的函数 属性 ，子类会覆盖父类的成员吗？ 不会
//如果子类与父类的成员函数名称相同，子类会把父类的所有的同名版本都隐藏掉
//想调用父类的方法，必须加作用域


int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}