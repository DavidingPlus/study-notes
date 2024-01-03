#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class A
{
public:
	A() {
		cout << "A()..." << endl;
		this->p = new char[64];
		memset(this->p, 0, 64);
		strcpy(this->p, "A String..");
	}

	virtual void print()
	{
		cout << "A: " << this->p << endl;
	}

	virtual ~A() {
		cout << "~A()..." << endl;
		if (this->p != NULL) {
			delete[]this->p;
			this->p = NULL;
		}
	}
private:
	char *p;
};

class B :public A
{
public:
	B() //此刻会触发A()
	{
		cout << "B()..." << endl;
		this->p = new char[64];
		memset(this->p, 0, 64);
		strcpy(this->p, "B String..");
	}

	virtual void print()
	{
		cout << "B: " << this->p << endl;
	}

	virtual ~B() {
		cout << "~B()..." << endl;
		if (this->p != NULL) {
			delete[] this->p;
			this->p = NULL;
		}
	}
private:
	char *p;
};


void func(A *ap)
{
	ap->print();//在此发生多态

}

void deleteFunc(A *ap)
{
	delete ap; //此刻ap->~B() //~B() ---> ~A()
}

void test()
{
	//A *ap = new A;
	//func(ap);
	B *bp = new B;
	func(bp);

	deleteFunc(bp);
}

int main(void)
{
	test();
	

	B bObj;

	//bObj.~B();
	return 0;
}