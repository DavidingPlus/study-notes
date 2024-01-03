#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class Parent
{
public:
	Parent()
	{
		cout << "Parent().." << endl;
		a = 0;
	}
	Parent(int a) {
		cout << "Parent(int)..." << endl;
		this->a = a;
	}
	~Parent(){
		cout << "~Parent" << endl;
	}
	int a;
};

class Child :public Parent
{
public:
	//在调用子类的构造函数时候，一定会调用父类的构造函数
	// 父类先构造，子类后构造。
	Child(int a, int b) :Parent(a)
	{
		cout << "Child(int, int)..." << endl;
		this->b = b;
	}

	void printC() {
		cout << "b = " << b << endl;
	}

	~Child(){
		cout << "~Child()..." << endl;
	}

	int b;
};

int main(void)
{
	Child c(10, 20);

	c.printC();
	
	return 0;
}