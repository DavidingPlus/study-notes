#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <functional>
using namespace std;

void test01(){

	plus<int> myplus;
	cout << myplus(10, 20) << endl;
}

int main(void){
	
	test01();

	return 0;
}