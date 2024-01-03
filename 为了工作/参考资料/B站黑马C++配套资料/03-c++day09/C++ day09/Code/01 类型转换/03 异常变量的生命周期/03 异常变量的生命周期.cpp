#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class MyException
{
public:
	MyException()
	{
		cout << "MyException的默认构造" << endl;
	}
	MyException(const MyException & e)
	{
		cout << "MyException的拷贝构造" << endl;
	}

	~MyException()
	{
		cout << "MyException的析构调用" << endl;
	}

	void printError()
	{
		this->m_A = 100;
		cout << "error"  << m_A<< endl;
	}

	int m_A;

};

void doWork()
{
	throw  &MyException();
}

void test01()
{
	try
	{
		doWork();
	}
	catch (MyException *e) //MyException e，会多开销一份数据
	{
		
		//e->printError();
		//e->printError();
		//e->printError(); //指向非法内存空间，不应该这么做

		cout << "捕获异常" << endl;

		//delete e; //靠自觉 释放对象
	}

}


int main(){


	test01();

	system("pause");
	return EXIT_SUCCESS;
}