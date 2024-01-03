#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class Complex
{
public:
	Complex(int a, int b)
	{
		this->a = a;
		this->b = b;
	}

	void printComplex()
	{
		cout << "( " << this->a << ", " << this->b << "i )" << endl;
	}

	friend ostream& operator<<(ostream & os, Complex &c);
	friend istream & operator>>(istream &is, Complex &c);

	//<<操作符只能写在全局，不能够写在成员方法中。否则调用的顺序会变饭，c1<<cout;
#if 0
	ostream& operator<<(ostream &os) //c1.operator<<(cout)
	{
		os << "( " << this->a << ", " << this->b << "i )";
		return os;
	}
#endif

private:
	int a;//实数
	int b;//虚数
};

#if 1
ostream& operator<<(ostream & os, Complex &c)
{
	os << "( " << c.a << ", " << c.b << "i )";

	return os;
}

istream & operator>>(istream &is, Complex &c)
{
	cout << "a:";
	is >> c.a;
	cout << "b:";
	is >> c.b;

	return is;
}
#endif

int main(void)
{
	Complex c1(1, 2);
	
	cin >> c1;//operaotr>>(cin, c1)

	cout << c1;
	//c1 << cout;
	//cout.operator<<(c1);

	//cout << c1 << "   " <<c1<< endl;//operator<<(cout, c1);
	
	return 0;
}