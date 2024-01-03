#pragma once
class programmer
{
public:
	programmer();
	virtual ~programmer();
public:

	virtual int getA()=0;
	virtual int getB() = 0;
	virtual int getC() = 0;
	virtual char *getName() = 0;

private:
	int a;
	int b;
	int c;
	char *name;
};
void GetSalary(programmer *pp);
