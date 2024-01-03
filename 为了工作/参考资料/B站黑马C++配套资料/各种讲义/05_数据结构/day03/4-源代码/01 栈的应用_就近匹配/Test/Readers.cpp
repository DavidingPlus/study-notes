#include<iostream>
#include<string>
using namespace std;
#include "Readers.h"

Reader::Reader()
{

}

int Reader::getnum()			 //获取读者学号函数
{
	return num;
}
string Reader::getgrade()	//获取读者年级函数
{
	return grade;
}
string Reader::getclas()	    	//获取读者班级函数
{
	return clas;
}
string Reader::getsex()			//获取读者性别函数
{
	return sex;
}
string Reader::getbook_name()	//获取所借书书名函数
{
	return book_name;
}
string Reader::getdate()	        //获取借书日期函数
{
	return date;
}
string Reader::getname()         //获取读者姓名函数
{
	return name;
}
int Reader::getavaiday()		//获取可借天数函数
{
	return avai_days;
}
int Reader::setname(string)		//设置读者姓名函数
{
	cout << "请输入读者姓名：" << endl;
	cin >> this->name;
	return 0;
}
int Reader::setnum(int)         //设置读者学号函数
{
	cout << "请输入读者的学号（学号从10001起，且不能和已有读者学号相同）：" << endl;
	cin >> this->num;
	return 0;
}
int Reader::setgrade(string)     //设置读者年级函数
{
	cout << "请输入读者年级：" << endl;
	cin >> this->grade;
	return 0;
}
int Reader::setclas(string)      //设置读者班级函数
{
	cout << "请输入读者班级：" << endl;
	cin >> this->clas;
	return 0;
}
int Reader::setsex(string)	     //设置读者性别函数
{
	cout << "请输入读者性别：" << endl;
	cin >> this->sex;
	return 0;
}
int Reader::setbook_name(string)	 //设置所借书籍书名函数
{
	cout << "请输入读者所借书名：" << endl;
	cin >> this->book_name;
	return 0;
}
int Reader::setdate(string)	     //设置借书日期函数
{
	cout << "请输入读者结束日期：" << endl;
	cin >> this->date;
	return 0;
}
int Reader::setavaiday(int)	     //设置可借天数函数
{
	cout << "请输入读者可借天数：" << endl;
	cin >> this->clas;
	return 0;
}
void Reader::book_out()			//借书函数
{

}
void Reader::book_in()			//还书函数
{

}
ostream& operator<<(ostream&out, Reader&anther)
{

	cout << "读者姓名：" << anther.name << endl;
	cout << "读者学号" << anther.num << endl;
	cout << "读者年级" << anther.grade << endl;
	cout << "读者班级" << anther.clas << endl;
	cout << "读者性别" << anther.sex << endl;
	if (anther.book_name != "未借书")
	{
		cout << "读者所借书名" << anther.book_name << endl;
		cout << "读者借书日期" << anther.date << endl;
		cout << "读者可借天数" << anther.avai_days << endl;
	}

	return out;
}
istream& operator>>(istream&in, Reader& anther)
{
	cout << "请输入读者姓名：" << endl;
	cin >> anther.name;
	cout << "请输入读者年级：" << endl;
	cin >> anther.grade;
	cout << "请输入读者班级：" << endl;
	cin >> anther.clas;
	cout << "请输入读者性别：" << endl;
	cin >> anther.sex;
	return in;
}