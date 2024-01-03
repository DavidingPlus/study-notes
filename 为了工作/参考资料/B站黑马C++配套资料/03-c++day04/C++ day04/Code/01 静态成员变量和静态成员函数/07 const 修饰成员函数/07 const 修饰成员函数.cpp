#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Person
{
public :
	Person()
	{
		//构造中修改属性
		//this 永远执行本体  
		
		this->m_A = 0;
		this->m_B = 0;
	}

	void showInfo() const //常函数 不允许修改指针指向的值
	{
		//this->m_A = 1000;
		this->m_B = 1000;
		// const Person * const this
		cout << "m_A = " << this->m_A << endl;
		cout << "m_B = " << this->m_B << endl;
	}

	void show2() const
	{
		//m_A = 100;
	}

	int m_A;
	mutable int m_B; //就算是常函数 我还是执意要修改

};

void test01()
{

	Person p1;
	p1.showInfo();

	//常对象 不允许修改属性
	const  Person p2;
	//cout << p2.m_A << endl;
	p2.show2();
	//常对象 不可以调用普通成员函数
	//常对象 可以调用常函数
}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}