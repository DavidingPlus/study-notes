#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

class MyString
{
public:

	MyString(const char * str)
	{
		//
	}

	explicit MyString(int a)
	{
		mSize = a;
	}

	char* mStr;
	int mSize;
};

void test01()
{
	
	MyString str = "abc";
	MyString str2(10);
	//MyString str3 = 10; //做什么用图？ str2字符串为 "10" 字符串的长度10 
	//隐式类型转换  Mystring str3 = Mystring (10);
	// explicit关键字 ，防止隐式类型转换

}

int main(){



	system("pause");
	return EXIT_SUCCESS;
}