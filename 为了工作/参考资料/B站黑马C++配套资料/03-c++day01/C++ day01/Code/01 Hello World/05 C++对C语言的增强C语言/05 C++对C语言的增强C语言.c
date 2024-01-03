#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//1、全局变量检测增强
int a;
int a = 10;

//2、函数检测增强
int getRectS(w, h)
{
}
void test02()
{
	getRectS(10, 10, 10);
}

//3、类型转换检测增强
void test03()
{
	char * p = malloc(sizeof(64)); //malloc返回值是void*
}

//4、struct 增强
struct Person
{
	int m_Age;
	//void plusAge(); //c语言中struct不可以加函数
};
void test04()
{
	struct Person p1; //使用时候必须加入struct关键字
}

//5、 bool类型增强 C语言中没有bool类型
//bool flag;

//6、三目运算符增强
void test06()
{
	int a = 10;
	int b = 20;

	printf("ret = %d \n", a > b ? a : b);

	//a > b ? a : b = 100; // 20 = 100 C语言返回的是值

	//C语言中想模仿C++写
	*(a > b ? &a : &b) = 100;
	printf("a = %d ,b = %d \n", a, b);

}

//7、 const增强
const int m_A = 10; //收到保护，不可以改
void test07()
{

	//m_A = 100;
	const int m_B = 20; //伪常量
	//m_B = 100;

	int * p = (int *)&m_B;
	*p = 200;
	printf("*p = %d , m_B = %d \n", *p, m_B);

	//int arr[m_B]; 不可以初始化数组

}



int main(){

//	test06();

	test07();

	system("pause");
	return EXIT_SUCCESS;
}