#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <string>
using namespace std;


//类模板
template <class NameType, class AgeType = int> //类模板可以有默认类型
class Person
{
public:
	Person(NameType name, AgeType age)
	{
		this->m_Name = name;
		this->m_Age = age;
	}

	void showPerson()
	{
		cout << "姓名：" << this->m_Name << " 年龄： " << this->m_Age << endl;
	}

	NameType m_Name;
	AgeType m_Age;
};
void test01()
{
	//自动类型推导 ，类模板 不支持
	//Person p("孙悟空", 100);

	//显示指定类型
	Person<string, int> p("孙悟空", 100);
	p.showPerson();
}
class Person1
{
public:
	void showPerson1()
	{
		cout << "Person1的调用" << endl;
	}
};

class Person2
{
public:
	void showPerson2()
	{
		cout << "Person2的调用" << endl;
	}
};

template<class T>
class myClass
{
public:
	T obj;
	void func1()
	{
		obj.showPerson1();
	}
	void func2()
	{
		obj.showPerson2();
	}
};
//类模板中成员函数 一开始不会创建出来，而是在运行时才去创建

void test02()
{
	myClass<Person1>m;

	m.func1();

	//m.func2();
}

int main(){

//	test01();

	test02();

	system("pause");
	return EXIT_SUCCESS;
}