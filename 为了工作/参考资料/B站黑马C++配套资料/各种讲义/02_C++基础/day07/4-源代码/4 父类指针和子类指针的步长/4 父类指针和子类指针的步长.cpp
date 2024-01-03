#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;


class Parent
{
public:
	Parent(int a)
	{
		this->a = a;
	}
	virtual void print()
	{
		cout << "Parent::print a=  " << a << endl;
	}

	int a;
};

class Child :public Parent
{
public:
	Child(int a) :Parent(a)
	{

	}

	virtual void print()
	{
		cout << "Child::print a=  " << a << endl;
	}

	int b;
};


int main(void)
{

	Child array[] = { Child(0), Child(1), Child(2) };
	
	Parent *pp = &array[0];
	Child *cp = &array[0];

	pp++;

	pp->print();
	cp->print();

#if 0
	pp++;//pp +sizeof(Parent)
	cp++;//cp +sizeof(Child)

	pp->print();
	cp->print();
#endif
	cout << "-----" << endl;

	int i = 0;
	for (cp= &array[0], i = 0; i < 3; i++, cp++) {
		cp->print();
	}

	return 0;
}