#pragma once
#include <iostream>
using namespace std;

//µ„¿‡
class Point
{
public:
	void setX(int x);
		/*{
		m_X = x;
		}*/
	void setY(int y);
		/*{
		m_Y = y;
		}*/

	int getX();
		/*{
		return m_X;
		}*/

	int getY();
	/*{
		return m_Y;
		}*/

private:
	int m_X;
	int m_Y;
};