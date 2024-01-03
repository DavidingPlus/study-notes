#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

// this可以解决命名冲突
class Person
{
public:
	Person(  int age)
	{
		this->age = age;
	}

	//对比年龄
	void compareAge( Person & p)
	{
		if (this->age == p.age )
		{
			cout << "年龄相等" << endl;;
		}
		else
		{
			cout << "年龄不相等" << endl;;
		}
	}

	//年龄相加
	 Person& PlusAge(Person & p)
	{
		 this->age += p.age;
		 return *this; //*this指向对象本体
	}

	int age;
};

void test01()
{
	Person p1(10);
	
	cout << "p1的年龄" << p1.age << endl;

	Person p2(10);

	p1.compareAge(p2);

	p1.PlusAge(p2).PlusAge(p2).PlusAge(p2); //链式编程

	cout << "p1的年龄" << p1.age << endl;
}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}