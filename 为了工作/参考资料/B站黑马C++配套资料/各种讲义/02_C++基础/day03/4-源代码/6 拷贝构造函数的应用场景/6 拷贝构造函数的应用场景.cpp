#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;

class Test
{
public:
	Test()
	{
		cout << "test()..." << endl;
		m_x = 0;
		m_y = 0;
	}
	Test(int x, int y)
	{
		cout << "Test(int x, int y)..." << endl;

		m_x = x;
		m_y = y;
	}
	Test(const Test & another)
	{
		cout << "Test(const Test &)..." << endl;
		m_x = another.m_x;
		m_y = another.m_y;
	}

	void operator=(const Test &another)
	{
		cout << "operatoer = (const Test &)" << endl;
		m_x = another.m_x;
		m_y = another.m_y;
	}

	void printT() {
		cout << "x = " << m_x << ", m_y = " << m_y << endl;
	}

	~Test() {
		cout << "~Test()..." << endl;
	}
private:
	int m_x;
	int m_y;
};


//析构函数调用的顺序， 跟构造相反， 谁先构造的，谁后析构。
//场景1
void test1()
{
	Test t1(10, 20);
	Test t2(t1);//Test t2 = t1;
}

//场景2
void test2()
{
	Test t1(10, 20);
	Test t2;

	t2 = t1;//=操作符
}


void func(Test t)//Test t = t1; //Test t 的拷贝构造函数
{
	cout << "func begin..." << endl;
	t.printT();
	cout << "func end..." << endl;
}

//场景3
void test3()
{
	cout << "test3 begin..." << endl;
	Test t1(10, 20);

	func(t1);

	cout << "test3 end..." << endl;
}


//场景4
Test func2()
{
	cout << "func2 begin..." << endl;
	Test temp(10, 20);
	temp.printT();

	cout << "func2 end..." << endl;

	return temp;
}//匿名的对象 = temp  匿名对象.拷贝构造(temp)

void test4()
{
	cout << "test4 being.. " << endl;
	func2();// 返回一个匿名对象。 当一个函数返回一个匿名对象的时候，函数外部没有任何
			//变量去接收它， 这个匿名对象将不会再被使用，（找不到）， 编译会直接将个这个匿名对象
			//回收掉，而不是等待整改函数执行完毕再回收.
	//匿名对象就被回收。
	
	cout << "test4 end" << endl;
}

void test5()
{
	cout << "test 5begin.. " << endl;
	Test t1 = func2(); //会不会触发t1拷贝构造来   t1.拷贝(匿名）？
						//并不会触发t1拷贝，而是 将匿名对象转正 t1，
						//把这个匿名对象 起了名字就叫t1.

	cout << "test 5 end.." << endl;
}

//场景6
void test6()
{
	cout << "test6 begin..." << endl;
	Test t1;//t1已经被初始化了。

	t1 = func2(); //t1已经被初始化了，所以func2返回的匿名对象不会再次转正，而依然是匿名对象。
					//所以t1会调用等号操作符，t1.operator=(匿名对象), 然后编译器会立刻回收掉匿名对象

	t1.printT();

	cout << "test6 end.." << endl;
}


int main(void)
{
	//test1();
	//test2();
	//test3();
	//test4();
	//test5();
	test6();

	return 0;
}