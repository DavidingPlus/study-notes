#pragma once
#include "Manager.h"
#include "SalesMan.h"

class SalesManager :public Manager, public SalesMan
{
public:
	SalesManager();
	~SalesManager();

	virtual void init();

	//提供员工薪资的计算方法
	virtual void getPay();

	//提供修改员工级别的方法
	virtual void upLevel(int level);
};

