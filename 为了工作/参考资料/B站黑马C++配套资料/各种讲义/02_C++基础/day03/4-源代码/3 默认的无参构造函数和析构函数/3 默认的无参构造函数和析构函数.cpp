#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class Test
{
public:
	//默认的无参构造函数
#if 0
	Test()
	{

	}
#endif
	//显示提供一个有参数的构造函数，默认的构造函数就不复存在
	Test(int x, int y)
	{
		m_x = x;
		m_y = y;
	}
	Test() {
		m_x = 0;
		m_y = 0;
	}

	void printT()
	{
		cout << "x = " << m_x << "  y = " << m_y << endl;
	}

	//默认的析构函数
#if 0
	~Test()
	{

	}
#endif
	~Test() {
		cout << "~Test()..." << endl;
	}


private:
	int m_x;
	int m_y;
};

int main(void)
{
	Test t1;//调用Test无参构造
	t1.printT();
	
	return 0;
}