#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <string>
using namespace std;

class Building
{
	//让全局的好基友函数 变为我的好朋友  友元函数
	friend void  goodGay(Building * building);
public:
	Building()
	{
		this->m_SittingRoom = "客厅";
		this->m_BedRoom = "卧室";
	}

	//客厅  卧室
public:
	string m_SittingRoom; //客厅

private:
	string m_BedRoom; //卧室
};

//全局函数  好基友
void  goodGay( Building * building )
{
	cout << "好基友正在访问 " << building->m_SittingRoom << endl;
	cout << "好基友正在访问 " << building->m_BedRoom << endl;
}
//友元函数 目的访问类中的私有成员属性
void test01()
{
	Building * building = new Building;
	goodGay(building);
}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}