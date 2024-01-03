#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;


//子类会继承父类的成员属性，成员函数
//子类不会继承父类的构造函数，析构函数
//只有父类自己知道自己构造函数 析构函数的属性，而子类不知道
class Base
{
public:
	Base()
	{
		int age;
		cout << "父类的构造函数调用" << endl;
	}
	~Base()
	{
		cout << "父类的析构函数调用" << endl;
	}
};

class Son :public Base
{
public:
	Son()
	{
		cout << "子类构造函数调用" << endl;
	}
	~Son()
	{
		cout << "子类析构函数调用" << endl;
	
	}


};







class Father
{
public:
	 
	Father(int a)
	{
	
		this->age = a;
		cout << "父类有参构造调用" << endl;
		cout << age << endl;
	}
	int age; 

};

class Son :public Father
{

public:
	Son(int a) :Father(a)
	{
		cout << "子类构造调用" << endl;
	}


		//int age;
};

void test()
{
	Son p1(10);
	
}





int main(){


	test();
	system("pause");
	return EXIT_SUCCESS;
}