#include "Employee.h"

int Employee::startNum = 1000;


Employee::Employee()
{
	cout << "Employee()..." << endl;
	id = startNum++;
	level = 1;
	salary = 0.0;
}


Employee::~Employee()
{
	cout << "~Employee().." << endl;
}


void Employee::displayStatus()
{
	cout << "员工姓名" << this->name << endl;
	cout << "员工的级别" << this->level << endl;
	cout << "员工的月薪 " << this->salary << endl;
	cout << "员工的ID" << this->id << endl;
}