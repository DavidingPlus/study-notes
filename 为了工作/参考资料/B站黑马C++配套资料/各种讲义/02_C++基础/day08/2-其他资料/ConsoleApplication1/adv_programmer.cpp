#include "adv_programmer.h"
#define _CRT_SECURE_NO_WARNINGS
#include"iostream"
using namespace std;


adv_programmer::adv_programmer()
{
	a = 7;
	b = 8;
	c = 9;
	name = new char[20];
	strcpy(name, "adv_programmer");
	
}


adv_programmer::~adv_programmer()
{
	if (name != NULL)
	{
		delete name;
		name = NULL;
	}
}
int		 adv_programmer::getA()
{
	return this->a;
}
int		 adv_programmer::getB()
{
	return this->b;
}
int		 adv_programmer::getC()
{
	return this->c;
}
char*	 adv_programmer::getName()
{
	return this->name;
}