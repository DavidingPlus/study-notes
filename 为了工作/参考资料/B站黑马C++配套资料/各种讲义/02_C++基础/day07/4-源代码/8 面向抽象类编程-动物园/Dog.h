#pragma once
#include "Animal.h"
class Dog : public Animal
{
public:
	Dog();
	~Dog();

	virtual void voice();
};

