#pragma once
#include <iostream>
#include<string>
#include"birthday.h"
#include"student.h"
#include"doctor.h"
#include"graduate.h"
using namespace std;

int main()
{
	Student s("zhaosi", 2001, 'm');
	s.dis();
	cout << "----------------" << endl;
	Graduate g("liuneng", 2001, 'x', 2000);
	g.dump();
	cout << "----------------" << endl;
	Doctor d("qiuxiang", 2001, 'y', 3000, "doctor", 2001, 8, 16);
	d.disdump();
	system("pause");
}