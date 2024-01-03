#pragma once
#include <string>
using namespace std;

class Reader{

	friend ostream& operator<<(ostream&out, Reader&anther);
	friend istream& operator>>(istream&in, Reader&anther);

public:
	Reader();
	int getnum();				 //获取读者学号函数
	string getgrade();		//获取读者年级函数
	string getclas();	    	//获取读者班级函数
	string getsex();			//获取读者性别函数
	string getbook_name();	//获取所借书书名函数
	string getdate();	        //获取借书日期函数
	string getname();         //获取读者姓名函数
	int  getavaiday();		//获取可借天数函数
	int setname(string); 		//设置读者姓名函数
	int setnum(int);         //设置读者学号函数
	int setgrade(string);     //设置读者年级函数
	int setclas(string);      //设置读者班级函数
	int setsex(string);	     //设置读者性别函数
	int setbook_name(string);	 //设置所借书籍书名函数
	int setdate(string);	     //设置借书日期函数
	int setavaiday(int);	     //设置可借天数函数
	void book_out();			//借书函数
	void book_in();			//还书函数

private:
	string name;                  //名字
	int	   num;                  //学号
	string grade;                 //年级
	string clas;                  //班级
	string sex;                   //性别
	string book_name;              //所借书的书名
	string date;                   //借书日期
	int avai_days;                  //可借天数
};

