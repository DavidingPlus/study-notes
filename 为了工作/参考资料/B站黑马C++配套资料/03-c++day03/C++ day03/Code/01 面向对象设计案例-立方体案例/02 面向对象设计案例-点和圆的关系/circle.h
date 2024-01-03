#pragma  once
#include <iostream>
using namespace std;
#include "point.h"

//圆类
class Circle
{
public:

	//设置半径
	void setR(int r);
	/*{
		m_R = r;
	}*/
	//获取半径
	int getR();
	/*{
		return m_R;
	}*/

	//设置圆心
	void setCenter(Point p);
	/*{
		m_Center = p;
	}*/
	//获取圆心
	Point getCenter();
	/*{
		return m_Center;
	}*/

	//利用成员函数判断点和圆关系
	void isInCircleByClass(Point & p);
	//{
	//	//获取圆心和点的距离 的平方
	//	int distance = (m_Center.getX() - p.getX()) * (m_Center.getX() - p.getX()) + (m_Center.getY() - p.getY()) * (m_Center.getY() - p.getY());
	//	int rDistance = m_R* m_R;
	//	if (rDistance == distance)
	//	{
	//		cout << "成员函数：：点在圆上" << endl;
	//	}
	//	else if (rDistance > distance)
	//	{
	//		cout << "成员函数：：点在圆内" << endl;
	//	}
	//	else
	//	{
	//		cout << "成员函数：：点在圆外" << endl;
	//	}
	//}

private:
	int m_R; //半径
	Point m_Center; //圆心
};