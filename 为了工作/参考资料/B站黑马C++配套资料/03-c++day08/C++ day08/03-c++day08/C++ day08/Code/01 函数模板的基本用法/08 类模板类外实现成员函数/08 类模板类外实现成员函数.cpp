#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include <string>

template<class T1 ,class T2>
class Person
{
public:
	Person(T1 name, T2  age);
	//{
	//	this->m_Name = name;
	//	this->m_Age = age;
	//}

	void showPerson();
	//{
	//	cout << "姓名：" << this->m_Name << "  年龄：  " << this->m_Age << endl;
	//}

	T1 m_Name;
	T2 m_Age;
};

//类外实现成员函数
template <class T1, class T2>
Person<T1, T2>::Person(T1 name, T2 age)
{
	this->m_Name = name;
	this->m_Age = age;
}

template <class T1, class T2>
void Person<T1, T2>::showPerson()
{
	cout << "姓名：" << this->m_Name << "  年龄：  " << this->m_Age << endl;
}

void test01()
{
	Person <string ,int> p1("Mt", 100);
	p1.showPerson();

}



int main(){

	test01();
	
	system("pause");
	return EXIT_SUCCESS;
}