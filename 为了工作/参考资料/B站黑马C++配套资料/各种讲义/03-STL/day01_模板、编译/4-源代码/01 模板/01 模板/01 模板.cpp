#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

#if 0 
//int类型数据交换
void MySwap(int& a, int& b){
	int temp = a;
	a = b;
	b = temp;
}

//double类型
void MySwap(double& a, double& b){
	double temp = a;
	a = b;
	b = temp;
}
#endif

//模板技术 类型参数化 编写代码可以忽略类型
//为了让编译器区分是普通函数  模板函数
template<class T1,class T2> //template<typename T>  //告诉编译器 我下面写模板函数，看到奇怪东西不要随便报错
void MySwap(T& a, T& b){
	T temp = a;
	a = b;
	b = temp;
}

void test01(){

	int a = 10;
	int b = 20;
	//1 自动类型推导
	cout << "a:" << a << " b:" << b << endl;
	MySwap(a, b); //编译器根据你传的值 进行类型自动推导
	cout << "a:" << a << " b:" << b << endl;

	double da = 1.13;
	double db = 1.14;
	cout << "da:" << da << " db:" << db << endl;
	MySwap(da, db);
	cout << "da:" << da << " db:" << db << endl;

	//2. 显式的指定类型
	MySwap<int>(a, b);
}

int main(void){
	
	test01();





	return 0;
}