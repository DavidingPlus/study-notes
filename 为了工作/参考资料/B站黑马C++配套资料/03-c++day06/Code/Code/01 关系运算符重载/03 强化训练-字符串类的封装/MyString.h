#pragma  once
#define  _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

class MyString
{
	friend ostream& operator<< (ostream& cout, MyString & str);
	friend istream& operator>> (istream& cin, MyString & str);
public:
	MyString(const char * str);
	MyString(const MyString & str);

	~MyString();

	//重载=运算符
	MyString& operator = (const char * str);
	MyString& operator=(const MyString & str);
	
	//重载[]运算符
	char& operator[](int index);

	//重载 +运算符
	MyString operator+(const char * str);
	MyString operator+(const MyString& str);

	//重载 == 运算符
	bool operator==(const char * str);
	bool operator==(const MyString & str);


private:
	char * pString; //执行堆区的指针
	int m_Size; // 字符串大小

};


