#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<vector>
#include<time.h>
#include<list>
#include<algorithm>
using namespace std;

//merge
struct MyPrint01{
	void operator()(int val){
		cout << val << " ";
	}
};

struct MyCompare01{
	bool operator()(int v1,int v2){
		return v1 > v2;
	}
};

void test01(){
	
	vector<int> v1;
	vector<int> v2;
	list<int> l3;

	srand((unsigned int)time(NULL));

	for (int i = 0; i < 10;i ++){
		v1.push_back(rand() % 10);
	}

	for (int i = 0; i < 10; i++){
		//v2.push_back(rand() % 10);
		l3.push_back(rand() % 10);
	}

	//默认从小到大
	sort(v1.begin(), v1.end(), MyCompare01());
	//sort(v2.begin(), v2.end(), MyCompare01());
	l3.sort(MyCompare01());

	//for_each(l3.begin(), v3.end(), MyPrint01());

	/*
		_OutIt merge(_InIt1 _First1, _InIt1 _Last1,
		_InIt2 _First2, _InIt2 _Last2,
		_OutIt _Dest)
	*/

	vector<int> v3;
	//v3.resize(v1.size() + v2.size());
	v3.resize(v1.size() + l3.size());

	//默认从小到大
	//merge(v1.begin(), v1.end(), v2.begin(), v2.end(), v3.begin(),MyCompare01());
	merge(v1.begin(), v1.end(), l3.begin(), l3.end(), v3.begin(), MyCompare01());
	for_each(v3.begin(), v3.end(), MyPrint01());

}

//random_shuffle
void test02(){
	//vector<int> v;
	list<int> v;  //自己写
	for (int i = 0; i < 9;i++){
		v.push_back(i);
	}

	//STL 语法丑陋不堪 但是效率非常高 一切考虑都是效率
	for_each(v.begin(), v.end(), MyPrint01()); cout << endl;
	random_shuffle(v.begin(), v.end()); //如果容器不支持随机访问，就不能用
	for_each(v.begin(), v.end(), MyPrint01()); cout << endl;

}

//reverse
void test03(){
	
	vector<int> v;
	//list<int> v;
	for (int i = 0; i < 9; i++){
		v.push_back(i);
	}

	for_each(v.begin(), v.end(), MyPrint01()); cout << endl;
	reverse(v.begin(), v.end());
	for_each(v.begin(), v.end(), MyPrint01()); cout << endl;
}


int main(void){

	//test01();
	//test02();
	test03();
	
	return 0;
}