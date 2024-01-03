#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <cstdlib>

using namespace std;


class Person
{
public:
	Person(char * name,int age)
	{
		this->name = name;
		this->age = age;
	}
	virtual void aryYouOK()
	{
		cout << "name: " << this->name << endl;
		cout << "age: " << this->age << endl;
	}
	string getName()
	{
		return name;
	}
	int getAge()
	{
		return age;
	}
private:
	string name;
	int age;
};

class Teacher : public Person
{
public:
	Teacher(char * name, int age, int wage) :Person(name, age)
	{
		this->wage = wage;
	}
	virtual void aryYouOK()
	{
		Person::aryYouOK();
		cout << "wage:" << this->wage << endl;
	}
private:
	int wage;
};

class Student:public Person
{
public:
	Student(char * name, int age, char * work) :Person(name, age)
	{
		this->work = work;
	}
	virtual void aryYouOK()
	{
		Person::aryYouOK();
		cout << "work:" << this->work << endl;
	}
private:
	string work;
};

void seeHello(Person & p)
{
	p.aryYouOK();
}


int main(void)
{
	Student stu("Íõ¶þ¹·", 18, "Ñ§Ï°");

	Teacher tea("°×½à",22, 8000);
	
	seeHello(stu);
	cout << endl;
	seeHello(tea);


	cout << endl;
	system("pause");
	return 0;
}