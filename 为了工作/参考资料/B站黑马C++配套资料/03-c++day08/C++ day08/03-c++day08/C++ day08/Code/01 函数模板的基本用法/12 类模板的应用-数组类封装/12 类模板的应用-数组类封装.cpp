#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
#include "MyArray.hpp"
#include <string>

//输出int类型数组
void printIntArray(  MyArray<int>& array)
{
	for (int i = 0; i < array.getSize();i++)
	{
		cout << array[i] << endl;
	}
}

class Person
{
public:
	Person(){};

	Person(string name, int age)
	{
		this->m_Name = name;
		this->m_Age = age;
	}
	string m_Name;
	int m_Age;
};



//输出Person类型数组
void printPersonArray( MyArray<Person> & array )
{
	for (int  i = 0; i < array.getSize(); i++)
	{
		cout << "姓名： " << array[i].m_Name << " 年龄： " << array[i].m_Age << endl;
	}
}



int main(){

	MyArray <int >arr(10);
	for (int i = 0; i < 10;i++)
	{
		arr.push_Back(i + 100);
	}

	printIntArray(arr);


	Person p1("MT", 10);
	Person p2("呆贼", 12);
	Person p3("傻馒", 14);
	Person p4("劣人", 15);

	MyArray<Person>arr2(10);
	arr2.push_Back(p1);
	arr2.push_Back(p2);
	arr2.push_Back(p3);
	arr2.push_Back(p4);

	printPersonArray(arr2);
	



	system("pause");
	return EXIT_SUCCESS;
}