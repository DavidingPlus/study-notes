#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Base
{
public:
	Base()
	{
		m_A = 10;
		cout << "Base默认构造函数调用" << endl;
	}
	~Base()
	{
		cout << "Base的析构函数调用" << endl;
	}

	int m_A;
};
// 子类会继承父类的成员属性，成员函数
//但是 子类 不会继承 父类 构造函数 和 析构函数
//只有父类自己知道如果构造和析构自己的属性，而子类不知道


class Son :public Base
{
public:
	Son()
	{
		cout << "Son默认构造函数调用" << endl;
	}
	~Son()
	{
		cout << "Son的析构函数调用" << endl;
	}
};

void test01()
{
	//Base b1;

	Son s1;
}



class Base2
{
public:
	Base2(int a)
	{
		this->m_A = a;
		cout << "有参构造函数调用" << endl;
	}
	int m_A;
};

class Son2:public Base2
{
public:
	Son2(int a ) : Base2(a)//利用初始化列表方式 显示调用 有参构造
	{
	}
};

void test02()
{
	Son2 s2(1000);
}


int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}