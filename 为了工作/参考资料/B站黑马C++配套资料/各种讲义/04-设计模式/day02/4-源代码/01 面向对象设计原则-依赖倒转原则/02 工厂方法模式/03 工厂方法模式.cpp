#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

//抽象水果
class AbstractFruit{
public:
	virtual void ShowName() = 0;
};

//苹果
class Apple : public AbstractFruit{
public:
	virtual void ShowName(){
		cout << "我是苹果!" << endl;
	}
};

//香蕉
class Banana : public AbstractFruit{
public:
	virtual void ShowName(){
		cout << "我是香蕉!" << endl;
	}
};

//鸭梨
class Pear : public AbstractFruit{
public:
	virtual void ShowName(){
		cout << "我是鸭梨!" << endl;
	}
};


//抽象工厂
class AbstractFruitFactory{
public:
	virtual AbstractFruit* CreateFruit() = 0;
};

//苹果工厂
class AppleFactory : public AbstractFruitFactory{
public:
	virtual AbstractFruit* CreateFruit(){
		return new Apple;
	}
};

//香蕉工厂
class BananaFactory : public AbstractFruitFactory{
public:
	virtual AbstractFruit* CreateFruit(){
		return new Banana;
	}
};

//鸭梨
class PearFactory : public AbstractFruitFactory{
public:
	virtual AbstractFruit* CreateFruit(){
		return new Pear;
	}
};

void test01(){
	
	AbstractFruitFactory* factory = NULL;
	AbstractFruit* fruit = NULL;

	//创建一个苹果工厂
	factory = new AppleFactory;
	fruit = factory->CreateFruit();
	fruit->ShowName();
	delete fruit;
	delete factory;

	//创建一个香蕉工厂
	factory = new BananaFactory;
	fruit = factory->CreateFruit();
	fruit->ShowName();
	delete fruit;
	delete factory;

	//创建一个鸭梨工厂
	factory = new PearFactory;
	fruit = factory->CreateFruit();
	fruit->ShowName();
	delete fruit;
	delete factory;

}

int main(void){

	test01();
	return 0;
}