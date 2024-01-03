#pragma once
#include "Employee.h"


class Manager : virtual public Employee
{
public:
	Manager();
	~Manager();

	//提供一个现实的初始化员工的函数
	virtual void init();

	virtual void getPay();

	//提供修改员工级别的方法
	virtual void upLevel(int level);

protected:
	double fixSalary;//经理的固定工资
};

