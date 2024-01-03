#pragma once
#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
using namespace std;
class Student
{
public:
	Student(string sn, int n, char s );
	~Student();
	void dis();

private:
	string name;
	int num;
	char sex;
};