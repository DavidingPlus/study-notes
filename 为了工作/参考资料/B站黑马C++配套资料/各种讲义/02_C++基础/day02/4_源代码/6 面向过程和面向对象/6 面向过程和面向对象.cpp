#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

class Dog
{
public:
	void eat(char *food)
	{
		cout << name << "吃" << food << endl;
	}

	char name[64];
};

//面向过程
void eat(class Dog &dog, char *food)
{
	cout << dog.name << "吃" << food << endl;
}




int main(void)
{
	Dog dog;

	strcpy(dog.name, "狗");

	eat(dog, "翔");

	dog.eat("翔");



	return 0;
}