#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include "test.h"


//C++中想调用C语言方法

//extern "C" void show(); //show方法 按照C语言方式做连接
//解决的问题就是 在C++中调用C语言的函数

int main(){

	show(); //在C++中 函数是可以发生重载的，编译器会把这个函数名称偷偷改变  _showv  void

	system("pause");
	return EXIT_SUCCESS;
}