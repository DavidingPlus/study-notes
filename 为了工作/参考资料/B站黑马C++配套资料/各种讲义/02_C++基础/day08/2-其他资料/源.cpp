#define _CRT_SECURE_NO_WARNINGS
#include<iostream>

using namespace std;

class programmer
{
public:
	programmer()
	{

	}
	virtual void wage() = 0;
	virtual ~programmer()
	{

	}
	
};

class unior_programmer :public programmer
{
public:
	virtual void wage()
	{
		cout << "初级程序员年薪10万!" << endl;
	}
};

class unior_mid_programmer :public programmer
{
public:
	virtual void wage()
	{
		cout << "中级程序员年薪50万!" << endl;
	}
};

class adv_programmer :public programmer
{
public:
	virtual void wage()
	{
		cout << "高级程序员年薪100万!" << endl;
	}
};

class Pay
{
public:
	Pay(programmer * programmer2)
	{
		programmer2->wage();

		if (programmer2 != NULL)
		{
			delete programmer2;
		}
	}

private:
	programmer *pro;

};

int main(void)
{ 
	
	
	Pay *p = new Pay(new unior_programmer);
	
	delete p;

	Pay *p1 = new Pay(new unior_mid_programmer);
	delete p1;

	Pay *p2 = new Pay(new adv_programmer);
	delete p2;
	
	system("pause");
	return 0;
}
