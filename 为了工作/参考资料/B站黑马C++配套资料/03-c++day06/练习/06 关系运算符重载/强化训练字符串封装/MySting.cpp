#include"MySting.h"


MyString::MyString(const char * str)
{
	//cout << "有参构造调用" << endl;
	this->pString = new char[strlen(str) + 1];
	this->m_Size=strlen(str);
	strcpy(this->pString, str);
}
MyString::MyString(const MyString & str)
{
	//cout << "拷贝构造引用调用" << endl;
	this->pString = new char[strlen(str.pString) + 1];
	this->m_Size = str.m_Size;
	strcpy(this->pString, str.pString);
}

MyString::~MyString()
{
	//cout << "析构调用" << endl;
	if (this->pString != NULL)
	{
		delete[]this->pString;
		this->pString = NULL;
	}
}

//重载=运算符
MyString& MyString::operator= (const char * str)
{
	//首先判断本类中的内容是否为空
	if (this->pString != NULL)
	{
		delete[]this->pString;
		this->pString = NULL;
	}

	this->pString = new char[strlen(str) + 1];

	strcpy(this->pString,str);
	return *this;
}

//重载=运算符
MyString& MyString::operator=(const MyString & str)
{
	if (this->pString != NULL)
	{
		delete[]this->pString;
		this->pString = NULL;
	}
	this->pString = new char[strlen(str.pString) + 1];

	strcpy(this->pString, str.pString);
	return *this;
}

MyString  MyString::operator+(const char * str)
{
	int size = this->m_Size + strlen(str) + 1;
	char *temp = new char[size];

	memset(temp, 0, size);
	strcat(temp, this->pString);
	strcat(temp, str);

	MyString nStr(temp);
	delete[]temp;
	return nStr;
}

MyString MyString::operator+(const MyString& str)
{

	int size = this->m_Size + strlen(str.pString) + 1;
	char *temp = new char[size];

	memset(temp, 0, size);
	strcat(temp, this->pString);
	strcat(temp, str.pString);

	MyString nStr(temp);
	delete[]temp;
	return nStr;
}


//重载[]运算符
char& MyString::operator[](int index)
{
	return this->pString[index];
}

//重载<<运算符
ostream& operator<< (ostream& cout, MyString & str)
{
	cout << str.pString;
	return cout;
}
//重载>>运算符
istream& operator>> (istream& cin, MyString & str)
{
	if (str.pString != NULL)
	{
		delete[]str.pString;
		str.pString = NULL;
	}

	char buf[1024];
	cin >> buf;
	str.pString = new char[strlen(buf) + 1];
	strcpy(str.pString, buf);
	return cin;

}

bool  MyString::operator==(const char * str)
{
	if (strcmp(str,this->pString)==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
bool  MyString::operator==(const MyString & str)
{
	if (strcmp(str.pString, this->pString) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void  MyString::getSize()
{

	cout << this->m_Size << endl;


}