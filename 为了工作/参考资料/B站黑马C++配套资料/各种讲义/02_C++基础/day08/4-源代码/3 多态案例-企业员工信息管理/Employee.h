#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
using namespace std;


//抽象的员工 
class Employee
{
public:
	Employee();
	virtual ~Employee();

	//提供一个现实的初始化员工的函数
	virtual void init() = 0;

	//提供员工薪资的计算方法
	virtual void getPay() = 0;
	//显示员工的信息
	void displayStatus();
	//提供修改员工级别的方法
	virtual void upLevel(int level) = 0;

protected:
	string name;//姓名
	int id;//编号
	double salary;//薪资
	int level;//级别

	//员工的编号基础值
	static int startNum;
};

