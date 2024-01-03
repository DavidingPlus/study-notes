#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

const double pi = 3.14;

// 1.	设计一个类，求圆的周长
// 周长公式  2 * pi * r
class Circle //class代表声明一个类 后面紧跟的是类的名称
{
public: //公共权限

//求圆的周长
	//在类里面写函数  成员函数
	double calculateZC()
	{
		return 2 * pi * m_R;
	}

	//设置半径的成员方法
	//成员函数 可以修改成员属性
	void setR(int r)
	{
		m_R = r;
	}

	//半径  成员属性
	int m_R;

};

void test01()
{
	//通过类 来创建一个圆 
	Circle c1; //圆 （对象）
	//c1.m_R = 10; //给这个对象来进行半径的赋值
	//通过成员函数 间接给圆设置半径
	c1.setR(10);

	//输出c1的周长

	cout << "c1的周长为： " << c1.calculateZC() << endl;
}


int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}