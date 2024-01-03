#include "SalesMan.h"


SalesMan::SalesMan()
{
	cout << "SaleMan()..." << endl;

}

 void SalesMan::init()
{
	 cout << "请输入销售员工的姓名" << endl;
	 cin >> this->name;
	 this->salePercent = 0.04;
}

SalesMan::~SalesMan()
{
	cout << "~SaleMan().." << endl;
}


//提供员工薪资的计算方法
void SalesMan::getPay()
{
	cout << "请输入当月的销售额" << endl;
	cin >> this->saleAmount;

	//计算当月的月薪
	this->salary = this->saleAmount *this->salePercent;
}

//提供修改员工级别的方法
void SalesMan::upLevel(int level)
{
	this->level += level;
	if (this->level == 1) {
		this->salePercent = 0.04;
	}
}
