#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

class A 
{
public:

	A(int a)
	{
		this->a = a;
	}

	void printA() {
		cout << "a = " << this->a << endl;
		B objB(3000);
		cout << objB.b << endl;
	}

	//声明一个友元类B
	friend class B;
private:
	int a;
};


class B
{
public:
	B(int b)
	{
		this->b = b;
	}
	void printB() {
		A objA(100);
		cout << objA.a << endl;
		cout << "b = " << this->b << endl;
	}
	friend class A;
private:
	int b;
};

int main(void)
{
	B bObj(200);

	bObj.printB();
	
	return 0;
}