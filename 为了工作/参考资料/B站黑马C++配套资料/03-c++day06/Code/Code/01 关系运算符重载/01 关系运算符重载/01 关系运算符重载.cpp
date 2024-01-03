#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <string>
using namespace std;


//  == 
class Person
{
public:
	Person(string name, int age)
	{
		this->m_Name = name;
		this->m_Age = age;
	}

	bool operator==( Person & p)
	{
		if (this->m_Name == p.m_Name && this->m_Age == p.m_Age)
		{
			return true;
		}
		return false;
	}

	bool operator!=( Person & p)
	{
		if (this->m_Name == p.m_Name && this->m_Age == p.m_Age)
		{
			return false;
		}
		return true;
	}

public:

	string m_Name;
	int m_Age;
};

void test01()
{
	Person p1("小明", 10);
	Person p2("小强", 15);
	Person p3("小强", 15);
	//int a = 10;
	//int b = 10;
	//if (a == b )
	//{
	//	cout << "a b相等" << endl;
	//}

	if ( p1 == p2)
	{
		cout << "p1 和 p2 相等" << endl;
	}
	else
	{
		cout << "p1 和 p2 不相等" << endl;
	}

	if (p2 == p3)
	{
		cout << "p2 和 p3 相等" << endl;
	}
	else
	{
		cout << "p2 和 p3 不相等" << endl;
	}

	if (p1 != p2)
	{
		cout << "p1 和 p2 不相等" << endl;
	}
	else
	{
		cout << "p1 和 p2 相等" << endl;
	}
}

int main(){

	test01();


	system("pause");
	return EXIT_SUCCESS;
}