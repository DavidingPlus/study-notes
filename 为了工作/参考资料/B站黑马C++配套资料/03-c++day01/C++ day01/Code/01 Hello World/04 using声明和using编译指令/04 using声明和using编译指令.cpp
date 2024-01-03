#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

namespace KingGlory
{
	int sunwukongId = 10;
}

void test01()
{
	int sunwukongId = 20;

	//using 声明  注意避免二义性问题
	//写了using声明后  下面这行代码说明以后看到的sunwukongId 是用KingGlory下的
	//但是  编译器又有就近原则
	//二义性
	//using KingGlory::sunwukongId;

	cout << sunwukongId << endl;
}

//using编译指令
namespace LOL
{
	int sunwukongId = 30;
}
void test02()
{
	//int sunwukongId = 20;
	//using编译指令
	using namespace KingGlory; //打开王者荣耀房间
	using namespace LOL;//打开LOL房间
	//如果打开多个房间，也要避免二义性问题
	cout << LOL::sunwukongId << endl;
}


int main(){

	//test01();
	test02();
	system("pause");
	return EXIT_SUCCESS;
}