#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include <string>

class MyOutOfRangeException:public exception
{

public:

	MyOutOfRangeException(string errorInfo)
	{
		this->m_ErrorInfo = errorInfo;
	}

	virtual  ~MyOutOfRangeException( )
	{

	}
	virtual const char *  what() const
	{
		//返回 错误信息
		//string 转 char *     .c_str()
		return this->m_ErrorInfo.c_str();
	}
	string m_ErrorInfo;
};
class Person
{
public:
	Person(string name, int age)
	{
		this->m_Name = name;
		//年龄做检测

		if (age < 0 || age > 200)
		{
			throw MyOutOfRangeException( string("我自己的年龄越界异常"));
		}
	}
	string m_Name;
	int m_Age;
};

void test01()
{
	try
	{
		Person p("张三", 300);
	}
	catch ( MyOutOfRangeException & e  )
	{
		cout << e.what() << endl;
	}
}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}