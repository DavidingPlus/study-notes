#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

class Animal
{
public:
	//纯虚函数，让子类继承并且实现
	virtual void voice() = 0;
	Animal();
	virtual ~Animal();
};


//架构函数
//让动物叫
void letAnimalCry(Animal *animal);
