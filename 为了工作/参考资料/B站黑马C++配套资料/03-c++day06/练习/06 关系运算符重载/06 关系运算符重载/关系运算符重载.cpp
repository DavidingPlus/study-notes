#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
using namespace std;

class Person
{
public:
	Person(){};
	Person(string str,int age)
	{
		this->m_str = str;
		this->age = age;
	}
	bool operator== (Person& p1)
	{
		if (p1.m_str == this->m_str&&this->age==p1.age)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	string m_str;
	int age;

};

void test()
{
	Person p1("狗子", 20);
	Person p2("狗子", 30);
	Person p3("狗子", 20);

	if (p1 == p3)
	{
		cout << "他们相同" << endl;
	}
	else
	{
		cout << "他们不相同" << endl;
	}

}



int main(){


	test();
	system("pause");
	return EXIT_SUCCESS;
}