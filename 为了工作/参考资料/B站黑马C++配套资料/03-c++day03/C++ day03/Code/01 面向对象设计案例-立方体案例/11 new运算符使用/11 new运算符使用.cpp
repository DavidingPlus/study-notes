#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Person
{
public:
	Person()
	{
		cout << "默认构造调用" << endl;
	}

	Person(int a)
	{
		cout << "有参构造调用" << endl;
	}

	~Person()
	{
		cout << "析构函数调用" << endl;
	}

};

void test01()
{
	//Person p1;  栈区开辟

	Person * p2 = new Person; //堆区开辟

	//所有new出来的对象 都会返回该类型的指针
	//malloc 返回 void* 还要强转
	//malloc会调用构造吗？ 不会  new会调用构造
	// new 运算符  malloc 函数
	//释放 堆区空间
	// delete也是运算符 配合 new用  malloc 配合 free用
	delete p2;

}

void test02()
{
	void *p = new Person(10); 
	//当用void* 接受new出来的指针 ，会出现释放的问题
	delete p;
	//无法释放p ，所以避免这种写法
}

void test03()
{
	//通过new开辟数组 一定会调用默认构造函数,所以一定要提供默认构造
	Person * pArray = new Person[10];
	//Person pArray2[2] = { Person(1), Person(2) }; //在栈上开辟数组，可以指定有参构造


	//释放数组 delete []
	delete [] pArray;
}

int main(){

	//test01();

	//test02();

	test03();

	system("pause");
	return EXIT_SUCCESS;
}