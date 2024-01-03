#pragma once
#include <iostream>
#include<string>
using namespace std;
class Birthday
{
public:
	Birthday(int y, int m, int d);
	~Birthday();
	void print();
private:
	int year;
	int month;
	int day;
};