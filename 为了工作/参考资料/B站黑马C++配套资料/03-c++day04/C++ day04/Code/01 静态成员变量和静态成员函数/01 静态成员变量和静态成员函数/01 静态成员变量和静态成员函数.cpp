#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Person
{
public:
	Person()
	{
		//m_Age = 10;
	}
	
	static int m_Age; //加入static就是 静态成员变量 ，会共享数据
	//静态成员变量，在类内声明，类外进行初始化

	//静态成员变量 也是有权限的
	int m_A;

	//静态成员函数
	//不可以访问  普通成员变量
	//可以访问 静态成员变量
	static void func()
	{
		//m_A = 10;
		m_Age = 100;
		cout << "func调用" << endl;
	};
	//普通成员函数 可以访问普通成员变量，也可以访问静态成员变量
	void myFunc()
	{
		m_A = 100;
		m_Age = 100;
	}
private:
	static int m_other; //私有权限 在类外不能访问

	static void func2()
	{
		cout << "func2调用" << endl;
	}
};
int  Person::m_Age = 0; //类外初始化实现
int  Person::m_other = 10;


void test01()
{
	//1 通过对象访问属性
	Person p1;
	p1.m_Age = 10;

	Person p2;
	p2.m_Age = 20;

	cout << "p1 = " << p1.m_Age << endl; //10 或者 20？ 20
	cout << "p2 = " << p2.m_Age << endl; //20
	//共享数据

	//2 通过类名访问属性
	cout << "通过类名访问Age" << Person::m_Age << endl;
	//cout << "other = " << Person::m_other << endl; //私有权限在类外无法访问

	//静态成员函数调用
	p1.func();
	p2.func();
	Person::func();

	//静态成员函数 也是有权限的
	//Person::func2();

}

int main(){

	test01();
	
	system("pause");
	return EXIT_SUCCESS;
}