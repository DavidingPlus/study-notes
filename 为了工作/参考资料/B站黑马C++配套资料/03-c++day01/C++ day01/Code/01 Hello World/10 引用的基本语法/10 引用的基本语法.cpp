#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//1、引用基本语法  Type &别名 = 原名
void test01()
{
	int a = 10;
	int &b = a;

	b = 20;

	cout << "a = " << a << endl;
	cout << "b = " << b << endl;
}
//2、引用必须初始化
void test02()
{
	//int &a; 必须初始化
	int a = 10;
	int &b = a; //引用初始化后不可以修改了
	int c = 20;

	b = c; //赋值！！！
}

//3、对数组建立引用
void test03()
{
	int arr[10];
	for (int i = 0; i < 10;i++)
	{
		arr[i] = i;
	}

	//给数组起别名
	int(&pArr)[10] = arr;
	for (int i = 0; i < 10;i++)
	{
		cout << pArr[i] << " ";
	}
	cout << endl;

	//第二种方式 起别名

	typedef int(ARRAYREF)[10]; //一个具有10个元素的int类型的数组
	ARRAYREF & pArr2 = arr;

	for (int i = 0; i < 10; i++)
	{
		cout << pArr2[i] << " ";
	}
	cout << endl;

}


int main(){

	//test01();

	test03();

	system("pause");
	return EXIT_SUCCESS;
}