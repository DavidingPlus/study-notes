#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;


class Test
{
public:
	Test(int value) {
		this->value = value;
	}

	Test operator+(Test &another)
	{
		cout << "执行了+操作符重载" << endl;
		Test temp(this->value + another.value);
		return temp;
	}

	bool operator&&(Test &another)
	{
		cout << "执行了&&操作符重载" << endl;
		if (this->value && another.value) {
			return true;
		}
		else {
			return false;
		}

	}

	bool operator||(Test &another)
	{
		cout << "重载了||操作符" << endl;
		if (this->value || another.value) {
			return true;
		}
		else {
			return false;
		}
	}

	~Test(){
		cout << "~Test()..." << endl;
	}
private:
	int value;
};

int main(void)
{
	int a = 1;
	int b = 20;


	Test t1(0);
	Test t2(20);

	//重载&&操作符，并不会发生短路现象。

	if (t1 && (t1+t2) ) {	//t1.operator&&(  t1.operator+(t2)  )
		cout << "为真" << endl;
	}
	else {
		cout << "为假" << endl;
	}

	cout << "------" << endl;

	if (t1 || (t1 + t2)) {//t1.operator||(  t1.operator+(t2) )
		cout << "为真" << endl;
	}
	else {
		cout << "为假" << endl;
	}


	return 0;
}