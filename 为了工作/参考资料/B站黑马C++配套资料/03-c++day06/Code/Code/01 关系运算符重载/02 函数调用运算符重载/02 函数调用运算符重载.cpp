#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <string>
using namespace std;

// ()重载

class MyPrint
{
public:
	void operator()( string text)
	{
		cout << text << endl;
	}
};

void test01()
{
	MyPrint myPrint;
	myPrint("hello world1111"); // 仿函数
}

class MyAdd
{
public:
	int operator()(int v1,int v2)
	{
		return v1 + v2;
	}

};
void test02()
{
	//MyAdd myAdd;
	//cout << myAdd(1, 1) << endl;

	cout << MyAdd()(1, 1) << endl; //匿名对象
}




int main(){

	//test01();

	test02();

	system("pause");
	return EXIT_SUCCESS;
}