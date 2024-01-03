#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include "Person.h"


//函数模板二次编译 会生成具体函数

int main(void){

	//让连接器找去吧
	Person<int> p(10); //构造函数定义在当前文件没有找到，编译认为这个函数在其他文件中定义
	//让链接器在链接的时候 去找这个函数的具体位置
	p.Show();

	
	return 0;
}