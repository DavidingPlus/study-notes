#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Father
{
public:

	Father()
	{
		this->m_A = 100;
	}
	void show()
	{
	
		cout << "基类调用" << endl;
	}

	int m_A;
};

class Son:public Father
{
public:

	Son()
	{
		this->m_A = 200;
	}

	void shou()
	{
		cout << "子类中函数调用" << endl;
	}

	int m_A;

};


void test()
{

	Son p1;
	cout << p1.m_A << endl;
	p1.show();
	p1.shou();


}





int main(){
	test();


	system("pause");
	return EXIT_SUCCESS;
}