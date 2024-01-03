#pragma once
#include<string>
using namespace std;

class Manager{

public:
	Manager();
	void Login();			//管理员登录
	void reader_1();		//读者管理界面函数
	void reader_add();	//增加读者函数
	void reader_del();	//删除读者函数
	void reader_mod();	//修改读者函数
	void book1();			//书籍管理界面函数
	void book_add();		//增加书籍函数
	void book_del();		//删除书籍函数
	void book_mod();		//修改书籍函数
	~Manager();
private:
	string Account;
	string Password;
};

class SupManager:public Manager
{
private:
	
public:
	SupManager();
	void manager_1();		//管理员管理界面函数
	void manager_add();	//增加管理员函数
	void manager_del();	//删除管理员函数
	void manager_mod();	//修改管理员函数
	~SupManager();
};

