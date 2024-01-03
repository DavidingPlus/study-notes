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

	//friend Complex & operator++(Complex &c);
	//friend const Complex operator++(Complex &c1, int);

	Complex &operator++()
	{
		this->a++;
		this->b++;

		return *this;
	}

	const Complex operator++(int)//亚元
	{
		Complex temp(this->a, this->b);
		this->a++;
		this->b++;
		return temp;
	}


private:
	int a;//实数
	int b;//虚数
};

#if 0
//重载的是前++运算符
Complex & operator++(Complex &c)
{
	c.a++;
	c.b++;
	return c;
}
#endif

//重载的是后++运算符
#if 0
const Complex operator++(Complex &c1, int)
{
	Complex temp(c1.a, c1.b);

	c1.a++;
	c1.b++;

	return temp;
}
#endif

int main(void)
{


	Complex c1(1, 2);

	//++++c1;

	c1++;

	c1.printComplex();

	//++++c1;

	return 0;
}