#pragma once
#include <iostream>
#include<string>
#include"student.h"
using namespace std;

class Graduate :public Student
{
	
public:
	Graduate(string sn, int in, char cs, float fs);
	~Graduate();
	void dump()
	{
		dis();
		cout << salary << endl;
	}
private:
	float salary;
};