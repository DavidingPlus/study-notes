#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class Test
{
public:
	Test(int k)
	{
		this->m_k = k;
	}

	int getK()  const//成员函数尾部出现const 修饰是this指针
	{
		//this->m_k = 100; //this指针不是 const Test *
		//this++;// this指针是一个常指针， Test *const  
		//this->m_k = 100;
		//this = this + 1;
		return this->m_k;
	}

	//static成员函数，只能返回static成员变量
	static int s_getK()
	{
		//return m_k;
		return s_k;
	}
private:
	int m_k;
	static int s_k;
};

int Test::s_k = 0;


int main(void)
{
	Test t1(10); //Test(&t1, 10);
	Test t2(20);


	return 0;
}