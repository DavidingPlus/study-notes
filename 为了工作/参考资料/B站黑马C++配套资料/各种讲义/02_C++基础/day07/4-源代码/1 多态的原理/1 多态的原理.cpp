#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class Parent
{
public:
	Parent(int a) {
		this->a = a;
	}

	virtual void func(int a)
	{
		cout << "Parent::func(int)..." << endl;
	}

	virtual void func(int a, int b, int c)
	{
		cout << "Parent::func(int ,int ,int )...." << endl;
	}
private:
	int a;
};

class Child :public Parent
{
public:
	Child(int a, int b) :Parent(a)
	{
		this->b = b;
	}
	virtual void func(int a) 
	{
		cout << "Child: func(int)..." << endl;
	}

	void func(int a, int b) {
		cout << "Child :func(int ,int )..." << endl;
	}

	virtual void func(int a, int b, int c)
	{
		cout << "Child ::func(int ,int ,int )..." << endl;
	}
private:
	int b;
};

void myFunc(Parent *pp)
{
	pp->func(10);
}

int main(void)
{
	//Parent *pp = new Parent(10);
	//Parent *cp = new Child(100, 200);


	Parent *pp = new Child(100, 200);

	pp->func(10);//Parent ？ Child
					//如果调用一个普通函数，编译器根本就不会查找虚函数表。
					//只有你调用的函数，是虚函数的时候，才会去查找虚函数表

	pp->func(10, 20, 30);

	return 0;
}