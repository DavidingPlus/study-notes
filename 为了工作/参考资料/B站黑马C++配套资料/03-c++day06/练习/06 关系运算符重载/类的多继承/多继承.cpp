#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
class father01
{
public:

	father01()
	{
		this->m_A = 10;
	}

	int m_A;
};


class father02
{
public:
	father02()
	{
		this->m_B = 20; 
		this->m_A = 6666;
	}
		int m_A;
		int m_B;
};



class Son :public father01, public father02
{
public:
	int c;
	int d;


};

//多继承会产生二义性
//加上作用域可以避免二义性
void test()
{

	Son p1;
	cout << "p1.m_A:" << p1.father02::m_A << endl;
	cout << "p1.m_B:" << p1.m_B << endl;
}




int main(){

	test();

	system("pause");
	return EXIT_SUCCESS;
}