#include "Manager.h"


Manager::Manager()
{
	cout << "Manager().." << endl;

}

void Manager::init()
{
	cout << "请输入经理的姓名" << endl;
	cin >> name;
	this->fixSalary = 8000;
}



Manager::~Manager()
{
	cout << "~Manager().." << endl;
}


void Manager::getPay()
{
	//计算薪水的方式
	this->salary = this->fixSalary;
}

//提供修改员工级别的方法
void Manager::upLevel(int level) {
	this->level += level;

	if (this->level == 1) {
		this->fixSalary = 8000;
	}
	else if(this->level == 2){
		this->fixSalary = 10000;
	}
}
