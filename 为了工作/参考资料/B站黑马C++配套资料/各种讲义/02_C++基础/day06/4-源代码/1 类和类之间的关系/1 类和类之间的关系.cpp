#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class A
{
public:
	void func() {
		cout << "funcA" << endl;
	}
	int a;
};

//类B拥有类A的成员变量， B has A ，//类B 依赖于 类A
class B
{
public:
	void funcB() {

	}
	A a;
};
//耦合度  高内聚 低耦合

//类C 的成员方法  需要类A的形参， C use A,//类C 依赖于 类A
class C
{
public:
	void funcC(A *a){

	}

	void funcC2()
	{

	}
};

//D继承与A    类D 如果是继承类A  类 D  is  A 。//类D继承与A 耦合度很高
class D :public A 
{
public:
	void funcD() {
		cout << this->a << endl;
	}

};

class E :public D
{

};
int main(void)
{
	

	return 0;
}