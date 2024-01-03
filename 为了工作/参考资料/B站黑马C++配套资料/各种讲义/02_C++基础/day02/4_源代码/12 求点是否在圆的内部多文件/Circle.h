#pragma once
#include "Point.h"

class Circle
{
public:
	void setXY(int x, int y);

	void setR(int r);

	//提供一个判断点是否在圆内
	//true 在内部
	//false 在外部
	bool judgePoint(Point &p);

private:
	int x0;
	int y0;
	int m_r;
};
