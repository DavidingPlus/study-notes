//编写一个C++程序, 计算程序员(programmer)工资
//1 要求能计算出初级程序员(junior_programmer) 中级程序员(
//mid_programmer)高级程序员(adv_programmer)的工资
//2 要求利用抽象类统一界面, 方便程序的扩展, 比如:新增, 计算 架构师
//(architect) 的工资

#define _CRT_SECURE_NO_WARNINGS
#include"iostream"
using namespace std;
#include"adv_programmer.h"
#include"mid_programmer.h"
#include"junior_programmer.h"


int main()
{
	programmer *pp=NULL;
	adv_programmer *ap = new adv_programmer;
	junior_programmer *jp = new junior_programmer;
	mid_programmer *mp = new mid_programmer;
	GetSalary(ap);
	ap = NULL;
	GetSalary(jp);
	jp = NULL;
	GetSalary(mp);
	mp = NULL;
	system("pause");
	return 0;
}