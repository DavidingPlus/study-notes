#include "mid_programmer.h"
#define _CRT_SECURE_NO_WARNINGS
#include"iostream"
using namespace std;

mid_programmer::mid_programmer()
{
	a = 3;
	b = 4;
	c = 5;
	name = new char[20];
	strcpy(name, "mid_programmer");
}


mid_programmer::~mid_programmer()
{
	if (name != NULL)
	{
		delete name;
		name = NULL;
	}
}

int mid_programmer::getA()
{

	return this->a;
}
int  mid_programmer::getB()
{
	return this->b;
}
int  mid_programmer::getC()
{
	return this->c;
}
char * mid_programmer::getName()
{
	return this->name;
}