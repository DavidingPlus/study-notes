#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Base
{
public:
	int m_A;
protected: 
	int m_B;
private:
	int m_C;

};

//子类中 会继承父类的私有成员，只是被编译给隐藏起来，访问不到私有成员
class Son :public Base
{
public:
	int m_D;
};

void test01()
{
	cout << sizeof(Son) << endl;

}

int main(){

	test01();

	system("pause");
	return EXIT_SUCCESS;
}