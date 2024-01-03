#include "junior_programmer.h"
#define _CRT_SECURE_NO_WARNINGS
#include"iostream"
using namespace std;


junior_programmer::junior_programmer()
{
	a = 5;
	b = 6;
	c = 7;
	name = new char[20];
	strcpy(name, "junior_programmer");
}


junior_programmer::~junior_programmer()
{
	if (name != NULL)
	{
		delete name;
		name = NULL;
	}
}

int		 junior_programmer::getA()
{
	return this->a;
}
int		 junior_programmer::getB()
{
	return this->b;
}
int		 junior_programmer::getC()
{
	return this->c;
}
char*	 junior_programmer::getName()
{
	return this->name;
}