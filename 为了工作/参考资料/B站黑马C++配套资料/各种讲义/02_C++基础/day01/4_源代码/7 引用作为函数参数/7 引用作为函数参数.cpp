#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

struct student
{
	int id;
	char name[64];
};

void my_swap(int *a, int *b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void my_swap2(int &a, int &b)
{
	int temp = a;
	a = b;
	b = temp;
}

void printS(struct student s) //student s = s1; 结构体整个值拷贝的动作
{
	cout << s.id <<" "<< s.name << endl;
	s.id = 100;
}

void printS1(struct student *sp)
{
	cout << sp->id << " " << sp->name << endl;
	sp->id = 100;
}

//引用在一定条件下能够取代指针的工作，也能作为函数参数。
void printS2(struct student &s)//student &s = s1;
{
	cout << s.id << "  " << s.name << endl;
	s.id = 300;
}

int main(void)
{
	int a = 10;
	int b = 20;

	my_swap2(a, b);
	cout << "a = " << a << endl;
	cout << "b = " << b << endl;


	student s1 = { 10, "zhang3" };

	printS(s1);
	printS1(&s1);
	printS2(s1);

	cout << "si.id =" << s1.id << endl;

	return 0;
}