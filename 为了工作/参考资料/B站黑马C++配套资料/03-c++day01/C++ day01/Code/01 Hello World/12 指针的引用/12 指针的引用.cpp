#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

struct Person
{
	int m_Age;
};

void allocatMemory(Person ** p) // **p 具体的Person对象  *p 对象的指针   p 指针的指针
{
	*p = (Person *)malloc(sizeof(Person));

	(*p)->m_Age = 100;
}

void test01()
{
	Person * p = NULL;
	allocatMemory(&p);
	cout <<  "p的年龄： " <<p->m_Age << endl;
}


//利用指针引用开辟空间
void allocatMemoryByRef(Person* &p)
{
	p = (Person*)malloc(sizeof(Person));
	p->m_Age = 1000;
}

void test02()
{
	Person * p = NULL;
	allocatMemoryByRef(p);
	cout << "p的年龄：" << p->m_Age << endl;
}


int main(){

	//test01();

	test02();

	system("pause");
	return EXIT_SUCCESS;
}