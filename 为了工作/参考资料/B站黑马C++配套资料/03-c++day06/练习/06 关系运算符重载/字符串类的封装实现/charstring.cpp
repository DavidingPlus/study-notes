#include"charstring.h"

String::String()
{
	cout << "默认构造调用" << endl;
}

String::String(char *name)
{
	this->name = new char[strlen(name) + 1];
	strcpy(this->name, name);
	this->size = strlen(name) + 1;
	//cout << "你输入的是:" << this->name << endl;

}
//拷贝构造函数
String::String(const String &p1)
{
	this->name = new char[strlen(p1.name)+1];
	strcpy(this->name, p1.name);
	this->size = p1.size;
}




String::~String()
{
	if (this->name != NULL)
	{
		delete[]this->name;
		this->name = NULL;
	}
	//cout << "析构调用" << endl;
}
//重载<<运算符
ostream& operator<<(ostream &cout, String &p)
{
	cout << p.name;
	return cout;
}
//重载>>符号
istream& operator>>(istream &cin, String &p)
{
	char buf[1024] = { 0 };
	cin >> buf;
	if (p.name != NULL)
	{
		delete[]p.name;
		p.name = NULL;
	}
	p.name = new char[strlen(buf) + 1];
	memset(p.name, 0, strlen(buf) + 1);
	strncpy(p.name, buf, strlen(buf) + 1);
	return cin;
}
//重载+运算，创建考贝构造
String String::operator+(String &p1)
{
	//这里已经加一
	int size = strlen(p1.name) + this->size + 1;

	char *temp = new char[size];

	memset(temp, 0, size);
	strcat(temp, this->name);
	strcat(temp, p1.name);
	
	String str(temp);
	//if (temp != NULL)
	
		delete[]temp;
		//temp = NULL;
	return str;
}
//==重载 引用版本
bool String::operator==(String &p1)
{
	if (strcmp(this->name, p1.name)==0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
//==重载 指针版本
bool String::operator==(char *p)
{
	if (strcmp(this->name, p) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//[]的重载
char String::operator[](int i)
{
	if (i >= this->size)
	{
		cout << "你输入的数大于字符长度" << endl;
		return NULL;
	}
	return this->name[i];
}

//赋值构造版本1
String & String::operator=(String &p1)
{
	if (this->name != NULL)
	{//先释放原有的内容
		//重新开开辟，传递进来字符串长度的内存
		delete[]this->name;
		this->name = NULL;
	}
	this->name = new char[strlen(p1.name) + 1];
	memset(this->name, 0, strlen(p1.name) + 1);
	strcpy(this->name, p1.name);
	return *this;
}

//赋值构造版本2
String & String::operator=(char *p)
{
	if (this->name != NULL)
	{
		delete[]this->name;
		this->name = NULL;
	}
	this->name = new char[strlen(p) + 1];
	memset(this->name, 0, strlen(p) + 1);
	strcpy(this->name, p);
	return *this;
}

char String::operator*()
{
	return *this->name;
}