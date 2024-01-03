#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

void mySwap(int a, int b)
{
	int tmp = a;
	a = b;
	b = tmp;

	cout << "mySwap::a = " << a << endl;
	cout << "mySwap::b = " << b << endl;
}

void test01()
{
	int a = 10;
	int b = 20;
	mySwap(a, b); //值传递

	cout << "a = " << a << endl;
	cout << "b = " << b << endl;
}


//地址传递
void mySwap2(int * a, int * b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}
void test02()
{
	int a = 10;
	int b = 20;
	mySwap2(&a, &b); 

	cout << "a = " << a << endl;
	cout << "b = " << b << endl;
}

//引用传递 类似传地址
void mySwap3(int &a, int &b)  //&a = a &b = b
{
	int tmp = a;
	a = b;
	b = tmp;
}

void test03()
{
	int a = 10;
	int b = 20;
	mySwap3(a, b); 

	cout << "a = " << a << endl;
	cout << "b = " << b << endl;
}

//引用的注意事项
//1、 引用必须引一块合法的内存空间
//2、不要返回局部变量的引用

int& doWork()
{
	int a = 10;
	return a;
}

void test04()
{
	//int &a = 10; // 引用必须引一块合法的内存空间
	int &ret = doWork();
	cout << "ret = " << ret << endl; //第一次10是编译器做了优化
	cout << "ret = " << ret << endl;
	cout << "ret = " << ret << endl;
	cout << "ret = " << ret << endl;
	cout << "ret = " << ret << endl;
}


int& doWork2()
{
	static int a = 10;
	return a;
}
void test05()
{
	
	int &ret = doWork2();
	
	//如果函数的返回值是引用，那么这个函数调用可以作为左值

	doWork2() = 1000; //相当于写了 a = 1000;

}

int main(){

	//test01();

	//test02();

	//test03();

	//test04();

	test05();

	system("pause");
	return EXIT_SUCCESS;
}