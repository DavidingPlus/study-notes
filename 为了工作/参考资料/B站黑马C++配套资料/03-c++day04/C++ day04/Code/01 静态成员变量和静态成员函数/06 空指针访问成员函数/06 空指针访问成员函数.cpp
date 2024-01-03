#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class Person
{
public:

	void show(  )
	{
		cout << "Person show" << endl;
	}
	void showAge()
	{
		if (this == NULL)
		{
			return;
		}
		cout << this->m_Age << endl; // NULL -> m_Age
	}

	int m_Age; // 
};

void test01()
{
	Person * p = NULL;
	p->show();
	p->showAge();

}

int main(){
	test01();


	system("pause");
	return EXIT_SUCCESS;
}