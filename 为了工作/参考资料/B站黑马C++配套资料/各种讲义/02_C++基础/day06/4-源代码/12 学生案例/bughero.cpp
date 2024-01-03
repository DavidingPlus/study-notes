#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>

using namespace std;

class jianzhu
{
public:
	jianzhu(string caizhi)

	{
		this->caizhi = caizhi;
	}
	virtual void getcaizhi()
	{
		cout << "建筑的材质是:" << caizhi << endl;
	}
	string caizhi;
};

class banfnag : public jianzhu
{
public:
	banfnag(string caizhi) : jianzhu(caizhi)
	{

	}
	virtual void getcaizhi()
	{
		cout << "板房的材质是:" << caizhi << endl;
	}
};
class gangjiegou : public jianzhu
{
public:
	gangjiegou(string caizhi) : jianzhu(caizhi)
	{
	
	}
	virtual void getcaizhi()
	{
		cout << "钢结构的材质是:" << caizhi << endl;
	}
};

void printC(jianzhu *jianzhu)
{
	jianzhu->getcaizhi();
}


int main(void)
{
	jianzhu j("水泥");
	banfnag b("木板");
	gangjiegou g("钢材");
	printC(&j);
	printC(&b);
	printC(&g);

	printf("\n");
	system("pause");
	return 0;
}