#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//异常基类
class BaseException
{
public:
	virtual void printError()
	{
	}
};

class  NullPointerException:public BaseException
{
public:
	virtual void printError()
	{
		cout << "空指针异常" << endl;
	}
};

class OutofRangeException:public BaseException
{
public:
	virtual void printError()
	{
		cout << "越界异常" << endl;
	}

};


void doWork()
{
	//throw NullPointerException();

	throw OutofRangeException();
}


void test01()
{
	try
	{
		doWork();
	}
	catch (BaseException & e)
	{
		e.printError();
	}

}


int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}