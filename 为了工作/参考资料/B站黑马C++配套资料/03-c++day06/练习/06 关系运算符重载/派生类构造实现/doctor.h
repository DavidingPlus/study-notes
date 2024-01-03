#pragma once
#include <iostream>
#include<string>
#include"birthday.h"
#include"student.h"
#include"doctor.h"
#include"graduate.h"
using namespace std;
class Doctor :public Graduate
{
public:
	Doctor(string sn, int in, char cs, float fs, string st, int iy, int im, int id);
	~Doctor();
	void disdump();
private:
	string title; //调用的默认构造器，初始化为””
	Birthday birth; //类中声明的类对象
};