#include<iostream>
using namespace std;

#include"Test.cpp"

int main()
{
	Test<int> t2(10);
	//t2.show();

	show2<int>(t2);


	system("pause");
	return 0;
}