#pragma once //防止头文件被重复包含
#include<iostream>
#include<string>

using namespace std;

class Person{
public:
	Person(string name, int age);
	void Show();
public:
	string mName;
	int mAge;
	int mID;
};
