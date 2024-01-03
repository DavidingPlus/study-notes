#pragma once
#include "programmer.h"
class mid_programmer :public programmer
{
public:
	mid_programmer();
	~mid_programmer();
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


