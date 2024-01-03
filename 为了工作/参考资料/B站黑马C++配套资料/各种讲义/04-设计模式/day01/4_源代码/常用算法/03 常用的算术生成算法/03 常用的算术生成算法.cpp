#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <numeric>
#include<vector>
#include<algorithm>
using namespace std;



struct MyPrint01{
	void operator()(int val){
		cout << val << " ";
	}
};
void test01(){

	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(9);
	v.push_back(3);

	int ret = accumulate(v.begin(), v.end(), 0);
	cout << "ret:" << ret << endl;

	//fill
	vector<int> v2;
	v2.resize(10);
	fill(v2.begin(),v2.end(),10);

	for_each(v2.begin(), v2.end(), MyPrint01());
}


void test02(){

	vector<int> v1;
	vector<int> v2;

	for (int i = 0; i < 10;i++){
		v1.push_back(i);
	}

	for (int i = 5; i < 15; i++){
		v2.push_back(i);
	}

	vector<int> v3;
	v3.resize(min(v1.size(),v2.size()));
	//交集
	vector<int>::iterator it = set_intersection(v1.begin(), v1.end(), v2.begin(), v2.end(), v3.begin());
	cout << "it:" << *it << endl;
	//打印结果
	for_each(v3.begin(), it, MyPrint01()); cout << endl;

	//求并集
	vector<int> v4;
	v4.resize(v1.size()+v2.size());
	it = set_union(v1.begin(),v1.end(),v2.begin(),v2.end(),v4.begin());
	for_each(v4.begin(), it, MyPrint01()); cout << endl;

	//差集
	vector<int> v5;
	v5.resize(v1.size());
	it = set_difference(v1.begin(), v1.end(), v2.begin(), v2.end(),v5.begin());
	for_each(v5.begin(), it, MyPrint01()); cout << endl;
	
}




int main(void){
	
	//test01();
	test02();
	return 0;
}