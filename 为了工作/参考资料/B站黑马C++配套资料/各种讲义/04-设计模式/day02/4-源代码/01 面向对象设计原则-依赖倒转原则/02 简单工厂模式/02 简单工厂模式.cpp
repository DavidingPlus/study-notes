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

//水果工厂
class FruitFactory{
public:
		static AbstractFruit* CreateFruit(string flag){
		if (flag == "apple"){

			return new Apple;
		}
		else if (flag == "banana"){
			return new Banana;
		}
		else if (flag == "pear"){
			return new Pear;
		}
		else{
			return NULL;
		}
	}
};

void test01(){
	

	//创建过程需要关心吗？ 直接拿来用

	//FruitFactory* factory = new FruitFactory;
	AbstractFruit* fruit = FruitFactory::CreateFruit("apple");
	fruit->ShowName();
	delete fruit;

	fruit = FruitFactory::CreateFruit("banana");
	fruit->ShowName();
	delete fruit;

	fruit = FruitFactory::CreateFruit("pear");
	fruit->ShowName();
	delete fruit;

	delete factory;

}




int main(void){
	test01();
	return 0;
}