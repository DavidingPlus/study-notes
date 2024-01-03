#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class Parent
{
public:
	virtual void func()
	{
		cout << "Parent::func().." << endl;
	}
	virtual void func(int a)
	{
		cout << "Parent::func().." << endl;
	}
private:
	int a;
};


class Parent2
{
public:
	void func()
	{
		cout << "Parent2::func().." << endl;
	}
private:
	int a;
};
int main(void)
{
	Parent p1;
	Parent2 p2;


	cout << "sizeof(p1) " << sizeof(p1) << endl;//多出来的4个字节就是vptr指针所占用的空间。
	cout << "sizeof(p2) " << sizeof(p2) << endl;

	
	return 0;
}