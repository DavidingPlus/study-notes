#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

class A
{
public:
#if 0
	A()
	{
		
	}
#endif
#if 0
	A(const A &another)
	{
		m_a = another.m_a;
		m_b = another.m_b;
	}
#endif
	A()
	{

	}
	A(int a, int b)
	{

	}
#if 0
	~A()
	{

	}
#endif
	~A()
	{
		cout << "~A()" << endl;
	}
private:
	int m_a;
	int m_b;
};

//类中 会有个默认的无参构造函数：	、
//		-->当没有任何***显示的构造函数（显示的无参构，显示有参，显示拷贝构造）*** 的时候，默认无参构造函数就会出现。

//		会有默认的拷贝构造函数：
//		-->当没有 **显示的拷贝构造 ***  的函数，默认的拷贝构造就会出现。

//     会有默认的析构函数
//      --> 当没有***显示的析构函数***的时候，  默认的析构函数就会出现。


int main(void)
{
	A a;
	
	A a1(a);
	
	return 0;
}