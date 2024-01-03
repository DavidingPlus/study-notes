#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
class Father
{

public:
	Father()
	{
		this->m_A = 10;
		cout << "父类的默认构造调用" << endl;
	}

	Father(int a)
	{
		this->m_A = a;
	}
	int m_A;

};

class Son :public Father
{
public:
	Son()
	{
		this->m_A = 777;
		cout << "子类构造调用" << endl;

	}
	int m_A;

};

void test()
{
	Son p1;
	//cout << p1.m_A << endl;
	cout << p1.Father::m_A << endl;

}




int main(){

		
	test();
	system("pause");
	return EXIT_SUCCESS;
}