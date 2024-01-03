#define _CRT_SECURE_NO_WARNINGS
#include"iostream"
using namespace std;

class Programmer
{
public:
	virtual int salaryPerMonth()=0;
	virtual char * getName()=0;
};

class CppProgrammer :public Programmer
{
public:
	virtual int salaryPerMonth()
	{
		return 20000;
	}
	virtual char *  getName()
	{
		return "CppProgrammer";
	}

};

class PhpProgrammer :public Programmer
{
public:
	virtual int salaryPerMonth()
	{
		return 10000;
	}
	virtual char *  getName()
	{
		return "PhpProgrammer";
	}

};

class JavaProgrammer :public Programmer
{
public:
	virtual int salaryPerMonth()
	{

		return 15000;
	}
	virtual char *  getName()
	{
		return "JavaProgrammer";
	}
};

class Girl
{
public:
	virtual int Beauty()
	{

	}
	virtual char *  getName() {

	}

};

class BaiFuMei : public Girl
{
public:
	virtual int Beauty()
	{
		return 19999;
	}
	virtual char *  getName()
	{
		return "BaiFuMei";
	}


};

class NvDiaoSi : public Girl
{
public:
	virtual int Beauty()
	{
		return 11000;
	}
	virtual char *  getName()
	{
		return "NvDiaoSi";
	}


};

class FengJie : public Girl
{
public:
	virtual int Beauty()
	{
		return 14000;
	}
	virtual char *  getName()
	{
		return "FengJie";
	}

};



void Marry(Programmer &pp, Girl &gg)
{
	if (pp.salaryPerMonth() > gg.Beauty())
	{
		cout << pp.getName() << "\t"<<"will marry   "<<gg.getName() << endl;
	}
	else

	{
		cout << "hey  " << pp.getName() << "  don't make a day dream! you want to marry to  " << gg.getName() <<"??"<< endl;
	}
}

int main()
{
	CppProgrammer cpp;
	PhpProgrammer php;
	JavaProgrammer java;
	BaiFuMei bfm;
	NvDiaoSi nds;
	FengJie fj;
	Marry(cpp, bfm);
	Marry(php, bfm);
	Marry(java, bfm);

	Marry(php, nds);
	Marry(java, bfm);

	
	Marry(java, fj);

	system("pause");
	return 0;
}