#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <memory>


using namespace std;

class A
{
public:
	A(int a)
	{
		cout << "A()..." << endl;
		this->a = a;
	}

	void func() {
		cout << "a = " << this->a << endl;
	}

	~A() {
		cout << "~A()..." << endl;
	}
private:
	int a;
};


class MyAutoPtr
{
public:
	MyAutoPtr(A * ptr)
	{
		this->ptr = ptr;//ptr = new A(10)
	}

	~MyAutoPtr() {
		if (this->ptr != NULL) {
			cout << "delte ptr" << endl;
			delete ptr;
			this->ptr = NULL;
		}
	}

	A* operator->()
	{
		return this->ptr;
	}


	A& operator*()
	{
		return *ptr;
	}

private:
	A *ptr;
};

void test1()
{
#if 0
	A* ap = new A(10);

	ap->func();
	(*ap).func();

	delete ap;
	auto_ptr<int> ptr(new int);
#endif
	auto_ptr<A> ptr(new A(10));

	ptr->func();
	(*ptr).func();
}


void test2()
{
	MyAutoPtr my_p(new A(10));

	my_p->func(); //my_p.ptr -> func()
	(*my_p).func(); //  *ptr.func()
}
int main(void)
{
	
	//test1();
	test2();

	return 0;
}