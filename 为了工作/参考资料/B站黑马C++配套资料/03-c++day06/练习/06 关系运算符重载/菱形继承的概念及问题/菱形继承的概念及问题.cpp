#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
class Animal
{
public:
	int m_Age;
};
//虚基类 Shep
class Sheep :virtual public Animal
{

};
class Tuo :virtual public Animal
{
};
class SheepTuo :public Sheep, public Tuo
{


};

void test()
{
	SheepTuo p1;
	p1.m_Age = 100;
	p1.Sheep::m_Age = 666;
	p1.Tuo::m_Age = 444;
	cout << p1.m_Age << endl;//可以直接访问，没有二义性

}



int main(){

	test();

	system("pause");
	return EXIT_SUCCESS;
}