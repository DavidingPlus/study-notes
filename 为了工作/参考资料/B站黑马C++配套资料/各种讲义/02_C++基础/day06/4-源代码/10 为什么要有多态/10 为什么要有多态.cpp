#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>

using namespace std;


//岳不群
class Yuebuqun
{
public:
	Yuebuqun( string kongfu)
	{
		this->kongfu = kongfu;
	}

	virtual  void fight() //标识修饰一个成员方法是一个虚函数。
	{
		cout << "岳不群" << "使出了" << kongfu << "打人" << endl;
	}

	void print()
	{

	}

	string kongfu;
};

//林平之 继承了 岳不群
class Linpingzhi :public Yuebuqun
{
public:
	Linpingzhi(string kongfu) :Yuebuqun(kongfu)
	{

	}

	//如果说父类中有一个虚函数是fight（ ）， 子类如果去重写这个虚函数。
	void fight()
	{
		cout << "林平之" << "使出了" << kongfu << "打人" << endl;
	}

	void print()
	{

	}
};

class Linghuchong :public Yuebuqun
{
public:
	Linghuchong(string kongfu) :Yuebuqun(kongfu)
	{

	}

	void  fight()
	{
		cout << "令狐冲 " << "使用了" << kongfu << endl;
	}
};

//在全局提供一个打斗的方法
void fightPeople(Yuebuqun *hero)//Yuebuqun *hero = xiaopp;  Yuebuqun *hero = xiaoyy;
{
	cout << "调用打人的方法" << endl;
	hero->fight();//希望传递进来的如果是子类，调用子类的fight
			      //如果传递进来的是父类， 调用父类的fight
					//这种行为就是 多态行为。
}

//多态发生的三个必要条件：
//1. 要有继承。
//2. 要有虚函数重写。
//3. 父类指针或引用 指向 子类对象。

int main(void)
{
	Yuebuqun *xiaoyy = new Yuebuqun("葵花宝典");

	//xiaoyy->fight();



	Linpingzhi *xiaopp = new Linpingzhi("僻邪剑谱");
	//xiaopp->fight();

	Linghuchong *xiaoll = new Linghuchong("独孤九剑");

	 
	fightPeople(xiaoyy);
	fightPeople(xiaopp);
	fightPeople(xiaoll);
	//编译器默认做了一个安全的处理。 编译认为 不管传递时子类对象还是父类对象，
	//如果统一执行父类d方法 那么是一定可以被成功执行。

	delete xiaoyy;
	delete xiaopp;
	delete xiaoll;

	return 0;
}