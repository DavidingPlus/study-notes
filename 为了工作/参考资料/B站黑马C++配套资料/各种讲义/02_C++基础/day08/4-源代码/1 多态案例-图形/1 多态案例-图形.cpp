#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;


//抽象的图形类
class Shape
{
public:
	//打印出图形的基本你属性
	virtual void show() = 0;
	//得到图形的面积
	virtual double getArea() = 0;

	virtual ~Shape() {

	}
};

//圆类
class Circle :public Shape
{
public:
	Circle(double r) {
		this->r = r;
	}

	//打印出图形的基本你属性
	virtual void show()  {
		cout << "圆的半径是 " << r << endl;
	}
	//得到图形的面积
	virtual double getArea()  {
		cout << "获取圆的面积" << endl;
		return this->r*this->r *3.14;
	}
	~Circle() {
		cout << "圆的析构函数。。" << endl;
	}
private:
	double r;
};

class Square :public Shape
{
public:
	Square(double a) {
		this->a = a;
	}

	//打印出图形的基本你属性
	virtual void show() {
		cout << "正方形的边长是" << this->a << endl;
	}
	//得到图形的面积
	virtual double getArea() {
		cout << "得到正方形的面积" << endl;
		return a*a;
	}


	~Square() {
		cout << "正方形的析构函数" << endl;
	}
private:
	double a;
};



int main(void)
{
	Shape *array[2] = { 0 };

	for (int i = 0; i < 2; i++) {
		//生成一个圆
		if (i == 0) {
			double r;
			cout << "请输入圆的半径" << endl;
			cin >> r;
			array[i] = new Circle(r);
		}
		//生成一个正方形
		else {
			double a;
			cout << "请输入正方形的边长" << endl;
			cin >> a;
			array[i] = new Square(a);
		}
	}


	//遍历这个array数组
	for (int i = 0; i < 2; i++) {
		array[i]->show();
		cout << array[i]->getArea() << endl;

		delete array[i];
	}

	return 0;
}