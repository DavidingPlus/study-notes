#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

int func(int a, int b)
{
	cout << " 1999 年写的 func" << endl;

	return 0;
}

int func2(int a, int b)
{
	cout << "1999 写的 func2" << endl;
	return 0;
}

int func3(int a, int b) 
{
	cout << "1999年 写的 func3 " << endl;
	return 0;
}

//2018想添加一个新的子业务
int new_func4(int a, int b)
{
	cout << "2018 新写的子业务" << endl;
	cout << "a = " << a << ", b = " << b << endl;
	return 0;
}

//方法一：  函数的返回值， 函数的参数列表（形参的个数，类型，顺序）
//定义一个函数类型。

typedef int(FUNC)(int, int);

//方法二：   定义一个函数指针
typedef int(*FUNC_P)(int, int);


//定义一个统一的接口 将他们全部调用起来。

void my_funtion(int(*fp)(int, int), int a, int b)
{
	cout << "1999年实现这个架构业务" << endl;
	cout << "固定业务1" << endl;
	cout << "固定业务2" << endl;

	fp(a, b);//可变的业务

	cout << "固定业务3" << endl;

}

int main(void)
{
#if 0
	//方法一：
	FUNC *fp = NULL;

	fp = func;
	fp(10, 20);

	FUNC_P fp2 = NULL;

	fp2 = func;

	fp2(100, 200);

	//方法三：
	int(*fp3)(int, int)   = NULL;
	fp3 = func;
	fp3(1000, 3000);
#endif
	my_funtion(func, 10, 20);
	my_funtion(func2, 100, 200);
	my_funtion(func3, 1000, 2000);

	my_funtion(new_func4, 2000, 3000);
	
	return 0;
}