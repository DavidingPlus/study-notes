#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include "MyString.h"

using namespace std;

int main(void)
{
	string s1;
	MyString s1("abc");
	MyString s2("123");

	//cout << s1 + s2 << endl;

	cout << s1 << endl;
	cout << s2 << endl;


#if 0
	MyString s1("abc");
	MyString s2(s1);
	MyString s3 = "123";


	cout << s1 << endl;
	cout << s2 << endl;

	s1[1] = 'x';

	cout << s1 << endl;

	s1 = s3;

	cout << s1 << endl;

#endif
	return 0;
}