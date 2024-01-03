#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <string>
using namespace std;

class Person
{

public:

	//设置年龄
	void setAge(int age)
	{
		if (age < 0 || age > 100)
		{
			cout << "你这个老妖精" << endl;
			return;
		}
		m_Age = age;
	}
	//获取年龄 读权限
	int getAge()
	{
		return m_Age;
	}

	//读姓名
	string getName()
	{
		return m_Name;
	}
	//写姓名
	void setName(string name)
	{
		m_Name = name;
	}

	//只写的情人
	void setLover(string lover)
	{
		m_lover = lover;
	}

private: //类外不可访问，类内可以访问

	int m_Age  = 0; //年龄 读写
	string m_Name; //公有权限  读写
	string m_lover; //情人  只写

};

void test01()
{
	Person p1;
	p1.setName("老王");

	cout << "p1的姓名：" << p1.getName() << endl;

	//年龄
	p1.setAge(120);

	cout << "p1的年龄：" << p1.getAge() << endl;

	//情人 只能设置 外部我不告诉你
	p1.setLover("仓井");

	


}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}