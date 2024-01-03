#include <iostream>
using namespace std;

class A
{
public:
	 void func(int data) { cout << "class A:" << data << endl; }
	 void func(char *str){ cout << "class A:" << str << endl; }
	void func(){ cout << "class A" << endl; }
};

class B : public A
{
public:
	void func(int data) { cout << "class B:" << data << endl; }
	void func(char *str, int i){ cout << "class B:" << str << " " << i << endl; }
	void func(){ cout << "class B" << endl; }
};

int main(int argc, char *argv[])
{
	A *pa;
	B b;
	pa = &b;
	pa->func(1);
	pa->A::func(1);
	pa->func("haha");
	b.func("haha", 2);
	pa->func();
	system("pause");
}
//class B: 1
//class A: 1
//class A; haha
//class B: haha 2
//class B