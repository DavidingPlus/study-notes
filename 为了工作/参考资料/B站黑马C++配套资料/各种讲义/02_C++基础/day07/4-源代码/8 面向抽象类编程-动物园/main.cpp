#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "Animal.h"
#include "Dog.h"
#include "Cat.h"


using namespace std;

int main(void)
{
	letAnimalCry(new Dog);

	letAnimalCry(new Cat);

#if 0
	Animal *dog = new Dog;
	letAnimalCry(dog);
	delete Dog;
#endif

	return 0;
}