#include "SalesManager.h"


SalesManager::SalesManager()
{
	cout << "SalesManager().." << endl;

}

void SalesManager::init()
{
	cout << "请输入销售经理的姓名" << endl;
	cin >> name;
	fixSalary = 5000;
	salePercent = 0.05;
}



SalesManager::~SalesManager()
{
	cout << "~SalesManager()..." << endl;
}

//提供员工薪资的计算方法
void SalesManager::getPay()
{
	cout << "请输入当月的销售额" << endl;
	cin >> this->saleAmount;

	this->salary = this->saleAmount*this->salePercent + this->fixSalary;
}

//提供修改员工级别的方法
 void SalesManager::upLevel(int level)
 {
	 this->level += level;
}
