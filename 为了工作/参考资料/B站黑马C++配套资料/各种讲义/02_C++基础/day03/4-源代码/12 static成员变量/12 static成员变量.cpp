#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

class AA
{
public:
	AA(int a, int b)
	{
		m_a = a;
		m_b = b;
	}



	int getC()
	{
		m_c++;
		return m_c;
	}
	
	//静态的成员方法
	static int& getCC()
	{
		return m_c;
	}

private:
	//static修饰的静态成员变量
	static int m_c;
	int m_a;
	int m_b;
};

//静态成员变量的初始化，一定类的外边。
int AA::m_c = 100;



int main(void)
{
	AA a1(10, 20);
	AA a2(100, 200);

	cout << a1.getC() << endl;//101
	cout << a2.getC() << endl;//102

	//a1.getCC() = 200;
	AA::getCC() = 200;

	cout << a1.getC() << endl;//201
	cout << a2.getC() << endl;//202

	return 0;
}