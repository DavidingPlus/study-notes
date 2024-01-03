#pragma once
#include "Animal.h"


class Cat : public Animal
{
public:
	Cat();
	~Cat();

	virtual void voice();
};

