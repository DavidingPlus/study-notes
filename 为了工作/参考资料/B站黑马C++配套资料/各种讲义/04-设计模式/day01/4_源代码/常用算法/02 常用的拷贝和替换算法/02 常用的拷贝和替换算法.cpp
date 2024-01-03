#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <vector>
#include<algorithm>
#include<time.h>
using namespace std;

struct MyPrint01{
	void operator()(int val){
		cout << val << " ";
	}
};


//copy swap
void test01(){

	vector<int> v1;
	srand((unsigned int)time(NULL));

	for (int i = 0; i < 10; i++){
		v1.push_back(rand() % 10);
	}

	/*
		_OutIt copy(_InIt _First, _InIt _Last,_OutIt _Dest)
	*/
	vector<int> v2;
	v2.resize(v1.size());
	copy(v1.begin(), v1.end(), v2.begin());

	for_each(v2.begin(), v2.end(), MyPrint01());

	//swap
	vector<int> v3;
	for (int i = 0; i < 10; i++){
		v3.push_back(rand() % 10);
	}
	cout << "--------------------" << endl;
	cout << "交换之前：" << endl;
	for_each(v3.begin(), v3.end(), MyPrint01()); cout << endl;
	for_each(v2.begin(), v2.end(), MyPrint01()); cout << endl;
	cout << "--------------------" << endl;
	swap(v3,v2);
	cout << "交换之后：" << endl;
	for_each(v3.begin(), v3.end(), MyPrint01()); cout << endl;
	for_each(v2.begin(), v2.end(), MyPrint01()); cout << endl;

}

//replace replace_if
struct MyCompare02{
	bool operator()(int val){
		return val > 5;
	}
};
void test02(){
	
	vector<int> v1;

	for (int i = 0; i < 10; i++){
		v1.push_back(i);
	}

	for_each(v1.begin(), v1.end(), MyPrint01()); cout << endl;
	replace(v1.begin(),v1.end(),5,100);
	for_each(v1.begin(), v1.end(), MyPrint01()); cout << endl;

	replace_if(v1.begin(), v1.end(), MyCompare02(), 200);
	for_each(v1.begin(), v1.end(), MyPrint01()); cout << endl;

}

int main(void) {

	//test01();
	test02();
	
	return 0;
}