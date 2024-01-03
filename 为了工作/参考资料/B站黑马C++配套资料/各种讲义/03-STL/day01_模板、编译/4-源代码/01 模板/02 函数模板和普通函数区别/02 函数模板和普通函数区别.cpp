#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

template<class T>
int MyAdd(T a,T b){
	return a + b;
}

//普通函数可以进行自动类型转换
//函数模板必须严格类型匹配
int MyAdd(int a,int c){
	return a + c;
}

void test01(){
	
	int a = 10;
	int b = 20;
	char c1 = 'a';
	char c2 = 'b';

	MyAdd<>(a,b);

#if 0 
	MyAdd(a,c1);
	MyAdd(a, b);
	MyAdd(c1,b);
#endif
}


template<class T>
void Print(T a){
	
}

template<class T>
void Print(T a , T b){

}

int main(void)
{
	test01();
	
	return 0;
}