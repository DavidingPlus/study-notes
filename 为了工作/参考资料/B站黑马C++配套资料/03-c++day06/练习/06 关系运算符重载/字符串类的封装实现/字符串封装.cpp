#include"charstring.h"


void test()

{
	String p1("狗子");
	//cout<<p1;
	//cin >> p1;
	cout << p1;

	String p2("aaaaa");
	String p3("aaaaa");
	String p4;
	//cout << p1 + p2<<endl;
	if (p3 == "aaaaa")
	{
		cout << "p3与p2相等" << endl;
	}
	else
	{
		cout << "p3与p2不等" << endl;
	}
	cout << p2[6] << endl;

	p4 = "kkkkkkk";
	cout << p4 << endl;

	cout << *(p4);
}
 
void main()
{
	test();
	system("pause");
	//cin.get();
}
