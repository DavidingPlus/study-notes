#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

//接口1
class Interface1
{
public:
	virtual void func1(int a, int b) = 0;
	virtual void func3(int a, int b) = 0;
};


//接口2
class Interface2
{
public:
	virtual void func2(int a) = 0;
};

class Child :public Interface1, public Interface2
{
public:
	virtual void func1(int a, int b)
	{
		cout << "func1" << endl;
	}
	virtual void func3(int a, int b) {
		cout << "func3" << endl;

	}


	virtual void func2(int a)
	{
		cout << "func2 " << endl;
	}
};

int main(void)
{
	Interface1 *if1 = new Child;

	if1->func1(10, 20);
	if1->func3(100, 200);


	Interface2 *if2 = new Child;

	if2->func2(10);

	return 0;
}