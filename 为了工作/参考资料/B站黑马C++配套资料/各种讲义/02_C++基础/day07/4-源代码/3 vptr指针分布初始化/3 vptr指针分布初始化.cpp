#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class Parent
{
public:
	Parent(int a)
	{
		cout << "Parent(int ..)" << endl;
		this->a = a;

		//print();//是调用父类的print（） 还是 子类的print（）？
				//通过结果 此时调用的父类的print 并不是子类的print
	}

	virtual void print()
	{
		cout << "Parent::print()...a = "<<a << endl;
	}
private:
	int a;
};

class Child :public Parent
{
public:
	Child(int a, int b) :Parent(a) //在调用父类的构造器的时候，会将vptr指针当做父类来处理。
									//此时会临时指向父类的虚函数表


	{
		//将子类对象的空间有编程子类对象处理，vptr指针就从指向父类的表 变成 指向子类的表

		cout << "Child (int ,int )" << endl;
		this->b = b;
		print();//此时vptr指针已经回到了 子类的表， 调用的是子类的print函数。

	}

	virtual void print() {
		cout << "Child ::Print()..b = " << b << endl;
	}
private:
	int b;
};

int main(void)
{
	Parent *pp = new Child(10, 20);
	//pp->print();//发生多态

	delete pp;
	
	return 0;
}