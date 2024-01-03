#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;


class Parent
{
public:
	Parent(int a) {
		this->a = a;
	}

	int a;
};

class Child :public Parent
{
public:
	Child(int p_a, int c_a) :Parent(p_a)
	{
		this->a = c_a;
	}

	void print()
	{
		cout << Parent::a << endl;
		cout << this->a << endl;//child's a
	}
	int a;
};


int main(void)
{
	Child c(10, 100);
	c.print();

	
	return 0;
}