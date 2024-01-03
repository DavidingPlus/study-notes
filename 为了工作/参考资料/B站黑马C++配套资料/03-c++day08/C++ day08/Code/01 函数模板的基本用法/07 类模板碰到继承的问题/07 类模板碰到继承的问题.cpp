#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

template <class T>
class Base
{
public:
	T m_A; //double类型
};

//child继承与 base必须告诉base中的T的类型，否则T无法分配内存
class Child :public Base<int>
{

};

//child2 也是模板类
template<class T1, class T2>
class Child2 :public Base<T2>
{
public:
	Child2()
	{
		cout << typeid(T1).name() << endl;
		cout << typeid(T2).name() << endl;
	}
public:
	T1 m_B; //int类型
};

void test01()
{
	Child2<int, double>child;//由用户指定类型

}


int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}