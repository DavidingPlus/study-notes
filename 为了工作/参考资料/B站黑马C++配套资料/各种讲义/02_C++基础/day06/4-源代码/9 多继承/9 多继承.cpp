#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

//家具类
class Furniture
{
public:
	int m; //材质
};

//将父亲类继承爷爷类  改成虚继承， 防止儿子在多继承我的时候，出现爷爷中的变量会拷贝多份。
class Bed:virtual public Furniture
{
public:
	void sleep() {
		cout << "在床上睡觉" << endl;
	}
};


class Sofa:virtual public Furniture
{
public:
	void sit() {
		cout << "在沙发上休息" << endl;
	}
};

//沙发床
class SofaBed :public Bed, public Sofa
{
public:
	void SleepAndSit() {
		sleep();
		sit();
	}
};

int main(void)
{
	Bed b;
	b.sleep();

	Sofa s;
	s.sit();

	cout << " ------ " << endl;

	SofaBed sb;
	sb.SleepAndSit();

	sb.m = 100;//此时只有一个m
	//sb.Bed::m = 100;
	//sb.Sofa::m = 200;
	
	return 0;
}