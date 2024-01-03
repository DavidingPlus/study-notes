#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Person
{
public:
	int m_A; //非静态成员变量，属于对象身上
	void func( ) {}; //非静态成员函数 不属于对象身上
	static int m_B; //静态成员变量 ，不属于对象身上
	static void  func2(){}; //静态成员函数 ，不属于对象身上
	double m_C; // 12 错误  16正确
};

//结论 ： 非静态成员变量 ，才属于对象身上

void test01()
{
	// 6 、 0 、 4 、 1
	cout << "sizo of (Person) = " << sizeof(Person) << endl;
	//空类的大小为 1  每个实例的对象 都有独一无二的地址，char维护这个地址
	// Person p[10]  p[0] p[1]
}


void test02()
{
	//this指针指向被调用的成员函数所属的对象

	Person p1;
	p1.func(); //编译器会偷偷 加入一个 this指针  Person * this

	Person p2;
	p2.func();
}

int main(){
	test01();


	system("pause");
	return EXIT_SUCCESS;
}