#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

/*
设计立方体类(Cube)，
求出立方体的面积( 2*a*b + 2*a*c + 2*b*c )和体积( a * b * c)，
分别用全局函数和成员函数判断两个立方体是否相等
*/

class Cube
{
public:
	//设置长
	void setL(int l)
	{
		m_L = l;
	}

	//获取长
	int getL() const
	{
		return m_L;
	}
	//设置宽
	void setW(int w)
	{
		m_W = w;
	}
	//获取宽
	int getW()
	{
		return m_W;
	}

	//设置高
	void setH(int h)
	{
		m_H = h;
	}
	//获取高
	int getH()
	{
		return m_H;
	}

	//求立方体的面积
	void getCubeS()
	{
		int s = 2 * m_L*m_W + 2 * m_W*m_H + 2 * m_L*m_H;
		cout << "立方体的面积为： " << s << endl;;
	}
	//求立方体体积
	void getCubeV()
	{
		int v = m_L * m_W * m_H;
		cout << "立方体的体积为： " << v << endl;
	}
	//通过成员函数判断是否相等
	bool compareCubeByClass( Cube & cub)
	{
		bool ret = m_L == cub.getL() && m_W == cub.getW() && m_H == cub.getH();
		return ret;
	}
private:
	int m_L; //长
	int m_W; //宽
	int m_H; //高
};

//为什么const刚刚不能添加？ 没法保证成员函数里是否修改了成员属性
void func(const Cube & cub )
{
	cub.getL();
}


//全局函数判断 两个立方体是否相等
bool compareCube( Cube &cub1,  Cube & cub2)
{
	if (cub1.getL() == cub2.getL()  && cub1.getW() == cub2.getW() && cub1.getH() == cub2.getH())
	{
		return true;
	}
	return false;

	
}

void test01()
{
	Cube c1;
	c1.setL(10);
	c1.setW(10);
	c1.setH(10);

	c1.getCubeS(); // 600
	c1.getCubeV(); // 1000

	//通过全局函数判断两个立方体是否相等

	Cube c2;
	c2.setL(10);
	c2.setW(10);
	c2.setH(10);

	bool ret = compareCube(c1, c2);
	if ( ret)
	{
		cout << "c1 和 c2 是相等的！" << endl;
	}
	else
	{
		cout << "c1 和 c2是不等的！" << endl;
	}

	//通过成员函数判断立方体是否相等
	bool ret2 = c1.compareCubeByClass(c2);
	if (ret2)
	{
		cout << "通过成员：：c1 和 c2 是相等的！" << endl;
	}
	else
	{
		cout << "通过成员：：c1 和 c2是不等的！" << endl;
	}
	
}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}