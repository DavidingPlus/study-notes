#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

class A
{
public:
	A()
	{
		cout << "A()..." << endl;
	}
	A(int a) {
		cout << "A(int)..." << endl;
		this->a = a;
	}

	//重载的new操作符 依然会触发对象的构造函数
	void * operator new(size_t size)
	{
		cout << "重载了new操作符" << endl;
		return malloc(size);
	}

	void *operator new[](size_t size)
	{
		cout << "重载了new[]操作符" << endl;
		return malloc(size);
	}
	void operator delete(void * p)
	{
		cout << "重载了delete操作符" << endl;
		if (p != NULL) {
			free(p);
			p = NULL;
		}
	}

	void operator delete[](void *p)
	{
		cout << "重载了delete[]操作符" << endl;
		if (p != NULL) {
			free(p);
			p = NULL;
		}
	}

	~A() {
		cout << "~A().... " << endl;
	}

private:
	int a;
};

int main(void)
{
	//char *array = malloc(sizeof(char)* 80);

	//int *value_p = new int;

	A *array_p = new A[10];


	//array_p->operator new[](sizeof(A[10]));
	delete[] array_p;

	A *ap = new A(10);

	//ap->operator new(sizeof(A));

	delete ap;
	
	return 0;
}