#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;

//大哥的虚衔
class BigBrother
{
public:
	//会打人。
	virtual void fightPeople() = 0;
};

//东方不败
class EastNeverLose :public BigBrother
{
public:
	virtual void fightPeople()  {
		cout << "使用了葵花宝典区打人" << endl;
	}
};

//无崖子
class Wuyazi :public BigBrother
{
public:
	virtual void fightPeople()  {
		cout << "使用北冥神功打人" << endl;
	}
};



//boss
int main(void)
{
	BigBrother *bigbrother = new Wuyazi;

	//大哥你给我去打人。
	bigbrother->fightPeople();

	delete bigbrother;
	
	return 0;
}