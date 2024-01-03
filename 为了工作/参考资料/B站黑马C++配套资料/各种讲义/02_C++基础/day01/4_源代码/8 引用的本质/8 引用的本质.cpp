#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

struct typeA
{
	int &a;
};

struct typeB
{
	int *a;
};


struct student
{
	int id;
	char name[64];
};

void motify(int *const a)//int *const a = main::&a
{
	*a = 300;
}

void motify2(int &a) //当我们将引用作为函数参数传递的时候，编译器，会替我们将实参，取地址给引用
					//int &a = main::&a
{
	a = 300; //对一个引用操作 赋值的时候，编译器提我们隐藏*操作。
}

//如果我们在去研究引用的时候，你可以将引用当做一个常指针去研究
//当你在使用引用编程的时候，你就把引用理解为变量的别名就可以了。


int main(void)
{
	cout << "sizeof(struct typeA)" << sizeof(struct typeA) << endl;
	cout << "sizeof(struct typeB)" << sizeof(struct typeB) << endl;
	//引用所占用的大小 跟指针是相等的。
	int a = 10;
	int &re = a; //常量要初始化，引用也要初始化， 引用可能是一刚常量。

	int *const p = &a;
	//综上两点， 引用 可能是一个常指针。


	motify(&a);

	motify2(a);

	return 0;
}