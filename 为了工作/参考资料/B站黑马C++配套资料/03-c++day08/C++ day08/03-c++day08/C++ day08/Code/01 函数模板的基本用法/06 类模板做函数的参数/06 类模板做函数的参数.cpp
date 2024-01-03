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

//1  指定传入类型
void doWork( Person<string ,int> & p ) 
{
	p.showPerson();
}

void test01()
{
	Person <string, int> p("MT",10);
	doWork(p);
}

//2 参数模板化
template<class T1 ,class T2>
void doWork2(Person<T1, T2> & p)
{
	//如何查看类型
	cout << typeid(T1).name() << endl;
	cout << typeid(T2).name() << endl;
	p.showPerson();
}
void test02()
{
	Person <string, int> p("呆贼", 18);

	doWork2(p);
}


//3 整体模板化
template<class T>
void doWork3(T&p)
{
	cout << typeid(T).name() << endl;
	p.showPerson();
}

void test03()
{
	Person <string, int> p("劣人", 18);

	doWork3(p);
}



int main(){

//	test01();

	test02();
	
	test03();

	system("pause");
	return EXIT_SUCCESS;
}