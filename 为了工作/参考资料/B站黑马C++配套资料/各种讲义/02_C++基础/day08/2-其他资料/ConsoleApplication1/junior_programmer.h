#pragma once
#include "programmer.h"
class junior_programmer :public programmer
{
public:
	junior_programmer();
	~junior_programmer();
public:
	int		 getA();
	int		 getB();
	int		 getC();
	char*	 getName();
private:
	int a;
	int b;
	int c;
	char *name;
};

