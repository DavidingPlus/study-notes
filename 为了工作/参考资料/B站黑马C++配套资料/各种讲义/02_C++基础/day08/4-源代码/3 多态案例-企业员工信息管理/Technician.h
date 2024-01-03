#pragma once
#include "Employee.h"


class Technician : public Employee
{
public:
	Technician();
	~Technician();

	virtual void init();

	//提供员工薪资的计算方法
	virtual void getPay();

	//升级的方法
	virtual void upLevel(int level);
private:
	//一个月工作了多少小时
	int workHour;
	//每小时多钱
	double perHourMoney;
};

