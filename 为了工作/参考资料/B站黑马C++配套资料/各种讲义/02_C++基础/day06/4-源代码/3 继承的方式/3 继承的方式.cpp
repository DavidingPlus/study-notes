#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <memory>

using namespace std;

//规则1， 只要是父类中的private成员，不管是什么继承方式，儿子都访问不了
//规则2，  如果是公有（public）继承， 儿子中的访问控制权限保持不变。
//规则3，  如果是保护(protected)继承, 儿子中父亲中除了private成员，其余在儿子中都是protected
//规则4，  如果是私有(private)继承， 儿子中的父亲的除了private成员，其余在二中都是private成员。

class Parent
{
public:
	int pub; //在类的内部 和 外部 都能访问。
protected:
	int pro; //在类的内部可以访问， 在类的外部不可以访问
private:
	int pri;  //在类的内部可以访问， 在类的外部不可以访问
};

//公有继承
class Child :public Parent
{
public:
	void func()
	{
		cout << pub << endl; //pub父类的public成员变量，在public继承 类的 【内部 外部】可以访问。

		cout << pro << endl;//pro 是父类protected成员变量 在public继承类的 【内部】可以访问。外部访问不了 
		                    //此时的pro在孙子能够访问，说此时pro不是private成员，而是protected成员

		//cout << pri << endl; //pri 是父类private成员变量 在public继承类的 【内部,外部】[不]可以访问。
	}

};

//孙子类
class SubChild : public Child
{
	void sub_func()
	{
		cout << pro << endl;
	}
};


//保护继承
class Child2 :protected Parent
{
public:
	void func2() {
		pub;//此时pub通过protected继承 能够在类的内部访问。 
			//pub 在类的内部可以访问， 类的外部访问不了， 类的儿子可以访问
			//pub 就是protected成员
		pro;//pro 根pub 是一样的性质，pro也是protected成员
		//pri;
	}
};

class Sub_child2:public Child2
{
public:
	void sub_func2() {
		pub;
		pro;
	}
};

//私有继承
class Child3 :private Parent
{
public:
	void func3()
	{
		pub;//pub 在类的内部可以访问。在类的内部可以访问，类的外部不能访问。
		    //pub 在儿子中访问不了，说明pub在Child3中是 私有成员
		pro;//pro 根pub的性质是一样， 也是私有成员。
		pri;
	}
};

class Sub_Child3 :public Child3
{
public:
	void sub_fun3()
	{
		pub;
		pro;
	}
};

//三看原则：  
//1 看调用的成员变量是在类的内部还是类的外部
//2 看儿子继承方式，
//3 当前变量在儿子中的变量在父亲中的访问控制权限

int main(void)
{
	Child c1;
	c1.func();

	c1.pub;
	//c1.pri;

	//Child2 c2;
	//c2.pub;
	//c2.pro;
	
	Child3 c3;
	//c3.pub;
	//c3.pro;

	Child2 c2;
	c2.pub;
	c2.pro;

	c1.pub;
	

	return 0;
}