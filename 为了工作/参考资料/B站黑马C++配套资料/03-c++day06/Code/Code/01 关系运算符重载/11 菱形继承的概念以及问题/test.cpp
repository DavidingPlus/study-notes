#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Animal
{
public:
	int m_Age;
};

//虚基类 Sheep
class Sheep :virtual public Animal
{
};
//虚基类 Tuo
class Tuo :virtual public Animal
{
};

class SheepTuo :public Sheep, public Tuo
{

};

//菱形继承的解决方案 利用虚继承
//操作的是共享的一份数据

void test01()
{
	SheepTuo st;
	st.Sheep::m_Age = 10;
	st.Tuo::m_Age = 20;

	cout << st.Sheep::m_Age << endl;
	cout << st.Tuo::m_Age << endl;
	cout << st.m_Age << endl; //可以直接访问，原因已经没有二义性的可能了，只有一份m_Age
}

//通过地址 找到 偏移量
//内部工作原理
void test02()
{
	SheepTuo st;
	st.m_Age = 100;

	//找到Sheep的偏移量操作
	//cout<< *(int *)((int *)*(int *)&st + 1) << endl;

	cout << *(int*)((int*)*(int *)&st + 1) << endl;

	//找到Tuo的偏移量
	cout << *((int *)((int *)*((int *)&st + 1) + 1)) << endl;
	
	//输出Age
	cout << ((Animal*)((char *)&st + *(int*)((int*)*(int *)&st + 1)))->m_Age << endl;

}

int main(){

	//test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}