#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;


//抽象车
class AbstractCar{
public:
	virtual void run() = 0;
};


//大众车
class Dazhong :public AbstractCar{
public:
	virtual void run(){
		cout << "大众车启动..." << endl;
	}
};

//拖拉机
class Tuolaji :public AbstractCar{
public:
	virtual void run(){
		cout << "拖拉机启动..." << endl;
	}
};


//针对具体类 不适用继承
#if 0
class Person : public Tuolaji{
public:
	void Doufeng(){
		run();
	}
};

class PersonB : public Dazhong{
public:
	void Doufeng(){
		run();
	}
};
#endif

//可以使用组合
class Person{
public:
	void setCar(AbstractCar* car){
		this->car = car;
	}
	void Doufeng(){
		this->car->run();
		if (this->car != NULL){
			delete this->car;
			this->car = NULL;
		}
	}
public:
	AbstractCar* car;
};

void test02(){
	
	Person* p = new Person;
	p->setCar(new Dazhong);
	p->Doufeng();

	p->setCar(new Tuolaji);
	p->Doufeng();

	delete p;
}


//继承和组合 优先使用组合
int main(void){

	test02();
	return 0;
}