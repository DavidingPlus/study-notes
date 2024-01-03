#include<iostream>
using namespace std;
class Test{
private:
	int n;
	static int m;
public:
	int p;
	static int q;
	Test(int arg);
	static void disp1();
	void disp2();
};
int Test::q = 20;
//ÉÙÒ»¾ä
int Test::m = 10;

Test::Test(int arg)
{
	n = arg;
	p = arg;
}

void Test::disp1()
{
	cout << "m = " << m << ", q = " << q << endl;
}

void Test::disp2()
{
	cout << "m = " << m << ", q = " << q << endl;
	cout << "n = " << n << ", p = " << p << endl;
}

int main()
{
	Test obj1(2);
	cout << "p = " << obj1.p << ", q = " << Test::q << endl;
	Test::disp1();
	obj1.disp2();
	system("pause");
	return 0;
}


//p = 2, q = 20
//m = 10, q = 20
//m = 10, q = 20
//n = 2, p = 2
 
