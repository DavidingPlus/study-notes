#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class Programmer
{
public:
	Programmer(double salary)
	{
		this->salary = salary;
	}
	virtual void printMoney() = 0;
	virtual ~Programmer() {

	}

protected:
	double salary;
};


class Junior_programmer :public Programmer
{
public:
	Junior_programmer(double salary) :Programmer(salary) {

	}
	virtual void printMoney(){
		cout << "初级程序员的工资是" << this->salary << endl;
	}
};

class Mid_programmer :public Programmer
{
public:
	Mid_programmer(double salary) :Programmer(salary) {

	}
	virtual void printMoney(){
		cout << "中级程序员的工资是" << this->salary << endl;
	}
};

class Adv_programmer :public Programmer
{
public:
	Adv_programmer(double salary) :Programmer(salary) {

	}
	virtual void printMoney(){
		cout << "高级程序员的工资是" << this->salary << endl;
	}
};





int main(void)
{

	Programmer * pro1 = new Junior_programmer(12000);

	pro1->printMoney();

	delete pro1;


	Programmer * pro2 = new Mid_programmer(15000);
	pro2->printMoney();
	delete pro2;

	Programmer *pro3 = new Adv_programmer(30000);
	pro3->printMoney();
	delete pro3;


	
	return 0;
}