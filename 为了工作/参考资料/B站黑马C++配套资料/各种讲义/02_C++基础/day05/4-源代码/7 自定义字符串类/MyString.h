#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

class MyString
{
public:
	MyString();
	//MyString(int len); //创建一个长度是len的string对象
	MyString(const char *str);
	MyString(const MyString &another);
	~MyString();

	//重载操作符[]
	char &operator[](int index);



	//重载操作符>>
	friend istream & operator>>(istream &is, MyString &s);

	//重载=操作符
	MyString & operator=(const MyString &another);

	//重载==操作符

	//重载!=操作符


	//重载+操作符
	MyString operator+(MyString &another);


	//重载操作符<<
	friend ostream & operator<<(ostream &os, MyString&s);

private:
	int len;
	char *str;
};

