#pragma once
#include "programmer.h"
class adv_programmer :public programmer
{
public:
	adv_programmer();
	~adv_programmer();
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

