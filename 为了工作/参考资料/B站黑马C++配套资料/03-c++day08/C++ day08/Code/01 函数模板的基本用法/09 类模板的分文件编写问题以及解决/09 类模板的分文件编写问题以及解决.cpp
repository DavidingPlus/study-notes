#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <string>
using namespace std;
#include "Person.hpp"

//建议 模板不要做分文件编写,写到一个类中即可,类内进行声明和实现，最后把后缀名改为.hpp 约定俗成 

int main(){

	Person<string, int>p("猪八戒", 10);
	p.showPerson();


	system("pause");
	return EXIT_SUCCESS;
}