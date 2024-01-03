#include "programmer.h"
#define _CRT_SECURE_NO_WARNINGS
#include"iostream"
using namespace std;

programmer::programmer()
{
}


programmer::~programmer()
{
}

void GetSalary(programmer *pp)
{
	int salary = pp->getA() * 1000 + pp->getB() * 2000 + pp->getC() * 3000;
	cout << pp->getName() << "  get :  " << salary << " /each month" << endl;
	delete pp;
}