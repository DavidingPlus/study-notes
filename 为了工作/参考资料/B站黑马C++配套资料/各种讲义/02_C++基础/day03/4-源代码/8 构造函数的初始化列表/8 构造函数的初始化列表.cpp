#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;


class A
{
public:
	A(int a)
	{
		cout << "A()..."<<a << endl;
		m_a = a;
	}

	~A() {
		cout << "~A()" << endl;
	}
	void printA() {
		cout << "a = " << m_a << endl;
	}


private:
	int m_a;
};


//构造函数的初始化列表
class B
{
public:
	
	B(A a1, A a2, int b) : m_a1(a1), m_a2(a2)
	{
		cout << "B(A&， A&, int)..." << endl;
		m_b = b;
	}

	//构造对象成员的顺序跟初始化列表的顺序无关
	//而是跟成员对象的定义顺序有关
	B(int a1, int a2, int b) : m_a1(a1), m_a2(a2)
	{
		cout << "B(int， int, int)..." << endl;

		m_b = b;
	}

	void printB() {
		cout << "b = " << m_b << endl;
		m_a1.printA();
		m_a2.printA();
	}

	~B()
	{
		cout << "~B().." << endl;
	}
private:
	int m_b;
	A m_a2;
	A m_a1;
};

void test1()
{
	A a1(10), a2(100);

	B b(a1, a2, 1000);

	b.printB();
}

class ABC
{
public:
	ABC(int a, int b, int c, int m) :m_m(m)
	{
		cout << "ABC(int ,int int)" << endl;
		m_a = a;
		m_b = b;
		m_c = c;

	}

	~ABC() {
		cout << "~ACB()" << endl;
	}

private:
	int m_a;
	int m_b;
	int m_c;
	const int m_m;//常量
};

class ABCD
{
public:
	ABCD(int a, int b, int c, int d, int m) : m_abc(a, b, c, m), m_d(d)
	{
		
	}

	ABCD(ABC&abc, int d) :m_abc(abc)
	{
		m_d = d;
	}
private:
	int m_d;
	ABC m_abc;
};

int main(void)
{
	B b(10, 20, 300);

	b.printB();

	ABC abc(10, 20, 30, 666);

	ABCD abcd(1, 2, 3, 4, 666);

	ABCD abcd1(abc, 40);
	
	return 0;
}