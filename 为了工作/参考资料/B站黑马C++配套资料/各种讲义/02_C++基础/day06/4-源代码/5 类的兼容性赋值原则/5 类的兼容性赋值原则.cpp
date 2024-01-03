#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

/*
	子类对象可以当作父类对象使用。
	子类对象可以直接赋值给父类对象。
	子类对象可以直接初始化父类对象.
	****父类指针可以直接指向子类对象***
	父类引用可以直接引用子类对象
*/

class Parent
{
public:
	void printP() {
		cout << "a " << this->a << endl;
	}
	int a;
};

class Child :public Parent
{
public:
	void printC()
	{
		cout << "b = " << this->b << endl;
	}
	int b;
};


void myPrint(Parent *pp)
{
	pp->printP();
}

int main(void)
{
//	Parent p;

//	Child c = p; //p对象填充不满c对象空间，

//	Child c;

//	Parent p = c;//c 对象所占用的内存空间 >= p对象占用空间 能够填充满p对象所需要空间。

//	p = c;

//	c.printP(); //c 能够当做父类 p 来使用。

	Parent *pp = NULL;//父类指针
	Child *cp = NULL;//子类指针
	

	Parent p;//父类对象
	Child c; //子类对象

	

	pp = &c;//c 内存布局能够满足父类指针的全部需求， 可以用一个儿子的对象地址给父类指针赋值。
	
	myPrint(&p);
	myPrint(&c);

	return 0;
}