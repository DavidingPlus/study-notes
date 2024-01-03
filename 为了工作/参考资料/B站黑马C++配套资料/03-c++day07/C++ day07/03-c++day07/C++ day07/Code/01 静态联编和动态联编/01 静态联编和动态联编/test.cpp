#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Animal
{
public:
	virtual void speak()
	{
		cout << "动物在说话" << endl;
	}

	virtual void eat()
	{
		cout << "动物在吃饭" << endl;
	}

};

class Cat :public Animal
{
public:
	void speak()
	{
		cout << "小猫在说话" << endl;
	}

	virtual void eat()
	{
		cout << "小猫在吃鱼" << endl;
	}
};

//调用doSpeak ，speak函数的地址早就绑定好了，早绑定，静态联编，编译阶段就确定好了地址
//如果想调用猫的speak，不能提前绑定好函数的地址了，所以需要在运行时候再去确定函数地址
//动态联编，写法 doSpeak方法改为虚函数,在父类上声明虚函数，发生了多态
// 父类的引用或者指针 指向 子类对象
void doSpeak(Animal & animal) //Animal & animal = cat
{
	animal.speak();
}
//如果发生了继承的关系，编译器允许进行类型转换

void test01()
{
	Cat cat;
	doSpeak(cat);

}


void test02()
{
	//cout << sizeof(Animal) << endl;
	//父类指针指向子类对象 多态
	Animal * animal = new Cat;

	//animal->speak();
	// *(int*)*(int*)animal 函数地址
	((void(*)()) (*(int*)*(int*)animal))();

	//  *((int*)*(int*)animal+1)猫吃鱼的地址

	((void(*)()) (*((int*)*(int*)animal + 1)))();
}

int main(){

	//test01();

	test02();

	system("pause");
	return EXIT_SUCCESS;
}