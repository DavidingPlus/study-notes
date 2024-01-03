#include <iostream>
#include <string>
using namespace std;


class Fu
{
public:
	Fu(string name)
	{
		this->name = name;
	}
	virtual void func()
	{
		cout << "调用了Fu的函数"<<endl;
		cout<<"Fu " << name <<" func()"<< endl;

	}
	string name;
};
class Zi :public Fu
{
	public:
	Zi(string name):Fu(name)
	{
	}
	void func()
	{
		cout << "调用了Zi的函数" << endl;
		cout << "Zi " << name << " func()"<< endl;
	}
	
};
class Zi2 :public Fu
{
	public:
	Zi2(string name) : Fu(name)
	{
		

	}
	void func()
	{
		cout << "调用了Zi2的函数" << endl;
		cout << "Zi2 "<< name << " func()" << endl;
	}
};
class Sun :public Zi
{
	public:
	Sun(string name) : Zi(name)
	{
	}
	void func()
	{
		cout << "调用了Sun的函数" << endl;
		cout << "Sun " << name << " func()"<<endl;
	}

};
void fun(Fu &f)
{
	f.func();//在此处应该发生多态

}
int main()
{
	Fu f("FFFF");
	Zi z("ZZZZ");
	Zi2 z2("TTTT");
	Sun s("SSSS");
	fun(f);
	fun(z);
	fun(z2);
	fun(s);
	return 0;
}
