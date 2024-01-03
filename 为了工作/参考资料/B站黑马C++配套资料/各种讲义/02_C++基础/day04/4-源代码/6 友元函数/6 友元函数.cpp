#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <cmath>


using namespace std;

class Point;

class PointManager {
public:
	double PointDistance(Point &p1, Point &p2);
};

class Point
{

public:
	//声明全局函数 PointDistance 是我类Point类的一个友元函数。
	//friend double PointDistance(Point &p1, Point &p2);
	friend double PointManager::PointDistance(Point &p1, Point &p2);

	Point(int x, int y) {
		this->x = x;
		this->y = y;
	}


	int getX()
	{
		return this->x;
	}

	int getY()
	{
		return this->y;
	}
private:
	int x;
	int y;

};

double PointManager::PointDistance(Point &p1, Point &p2)
{
	double dis;
	int dd_x = p1.x - p2.x;
	int dd_y = p1.y - p2.y;

	dis = sqrt(dd_x*dd_x + dd_y *dd_y);

	return dis;
}

# if 0
double PointDistance(Point &p1, Point &p2)
{
	double dis;
	int dd_x = p1.x - p2.x;
	int dd_y = p1.y - p2.y;

	dis = sqrt(dd_x*dd_x + dd_y *dd_y);

	return dis;
}
#endif



int main(void)
{
	
	Point p1(1, 2);
	Point p2(2, 2);

	//cout << PointDistance(p1, p2) << endl;

	PointManager pm;
	cout << pm.PointDistance(p1, p2) << endl;

	return 0;
}