#pragma once
#include "Employee.h"
class SalesMan : virtual public Employee
{
public:
	SalesMan();
	~SalesMan();

	//提供员工薪资的计算方法
	virtual void getPay();

	virtual void init();

	//提供修改员工级别的方法
	virtual void upLevel(int level);

protected:
	//当月的销售额
	int saleAmount;
	//提成的比率
	double salePercent;
};

