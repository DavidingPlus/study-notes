#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
using namespace std;

//把大象关进冰箱
//冰箱类
class IceBox
{
protected:
	int size;//冰箱的容积
public:
	IceBox(int size)
	{
		this->size = size;
	}
	virtual int getSize()
	{
		return this->size;
	}
};

class Animal
{
protected:
	int size;
public:
	Animal(int size)
	{
		this->size = size;
	}
	virtual int getSize()
	{
		return this->size;
	}

};
//大象类
class Elephent:public Animal
{
private:
	string name;
public:
	Elephent(int size, string name) :Animal(size)
	{
		this->name = name;
	}
	virtual int getESize()
	{
		return this->size;
	}
	string getName()
	{
		return this->name;
	}
};

class Geli:public IceBox
{
private:
	string name;
public:
	
	Geli(int size , string name) :IceBox(size)
	{
		this->name = name;
	}
	virtual int getSize()
	{
		return this->size;
	}
	string getName()
	{
		return this->name;
	}
};

void putEleIntoBox(IceBox *ib, Animal *an)
{
	if (ib->getSize() > an->getSize())
	{
		cout << "把动物装进去了" << endl;
	}
	else
	{
		cout << "动物卡住了" << endl;
	}
}
int main()
{
	
	IceBox ib(100);
	Animal an(200);


	putEleIntoBox(&ib, &an);

	Elephent *ep = new Elephent(200, "非洲象");
	Geli *DongMZ = new Geli(300, "geli");

	putEleIntoBox(DongMZ, ep);
	system("pause");
	return 0;
}