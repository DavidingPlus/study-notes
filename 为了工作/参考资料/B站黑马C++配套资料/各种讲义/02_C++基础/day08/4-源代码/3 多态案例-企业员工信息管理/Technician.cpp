#include "Technician.h"


Technician::Technician()
{
	cout << "Technician().." << endl;

}

void Technician::init()
{
	cout << "请输入员工的姓名" << endl;
	cin >> name;
	perHourMoney = 100;//默认员工每小时赚100元
}


Technician::~Technician()
{
	cout << "~Technician()..." << endl;
}

//提供员工薪资的计算方法
void Technician::getPay()
{
	cout << "请输入该员工 一共工作了多少小时" << endl;
	cin >> workHour;

	this->salary = this->perHourMoney * this->workHour;
}

//升级的方法
void Technician::upLevel(int level)
{
	this->level += level;

}

