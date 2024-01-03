#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;


class Berson
{
public:
	int a;
private:
	int b;



};


class  Son :public Berson
{
public:
	int d;
};

void test()
{
	cout << sizeof(Son) << endl;
}




int main(){


	test();
	system("pause");
	return EXIT_SUCCESS;
}