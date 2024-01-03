#include <iostream>
#include <string>
using namespace std;
class jungry
{
private:
	jungry()
	{
		count = 0;
	}
	static jungry *p;
	int count;
public:
	static jungry*getP()
	{
		return p;
	}
	void print(string content)
	{
		cout << "´òÓ¡ÄÚÈÝÎª£º" << content << endl;
		count++;
	}

	int GetCount()
	{
		return this->count;
	}
};

jungry*jungry::p = new jungry;


int main(void)
{
	jungry *p = jungry::getP();
	jungry *p1 = jungry::getP();
	p->print("111");
	p->print("222");
	p->print("333");
	cout << p->GetCount();
	delete p;

	//(*(int*)p) = 10;

	//cout << *(int*)p << endl;



	p1->print("4");
	p1->print("5");
	p1->print("6");
	cout << p1->GetCount();
	return 0;
}
