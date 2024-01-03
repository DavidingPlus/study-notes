#include "Books.h"
#include <iostream>
#include <vector>
using namespace std;

Books::Books()    //book类构造函数
{
	this->name = "book";
	this->writer = "writer";
	//this->vborrower;
	this->id = 0;
	this->store = 0;
	this->total = 0;
	this->pub_time = "pub_time";
	this->price = 0.0;
	this->publish = "publish";
}

void  Books::set_id()		//设置书号
{
	cout << "请输入书籍编号（书籍编号从10001起，且不能和已有书籍重号）：" << endl;
}
string Books::get_name()           //获取书名
{
	return name;
}
string Books::get_writer()		  //获取作者
{
	return writer;
}

vector<Reader>& Books::get_vborrower()			  //获取借书人
{
	return vborrower;
}

int Books::get_id()				 //获取书号
{
	return id;
}

int Books::get_store()			 //获取现存量
{
	return store;
}
int Books::get_total()			 // 获取总存量
{
	return total;
}
string Books::get_pub_time()		 //获取出版时间
{
	return pub_time;
}
float  Books::get_price()	//获取价格
{
	return price;
}
string Books::get_publish()		 //获取出版社
{
	return publish;
}

ostream& operator<<(ostream&out, Books&anther)
{

	cout << "书名：" << anther.name;
	cout << "作者：" << anther.writer;
	cout << "书号：" << anther.id;
	cout << "现存量：" << anther.store;
	cout << "总数量：" << anther.total;
	cout << "出版时间：" << anther.pub_time;
	cout << "价格：" << anther.price;
	cout << "出版社：" << anther.publish;
	if (anther.total - anther.store > 0)
	{
		for (int i = 0; i < anther.total - anther.store; i++)
			cout << "借书读者" << i << "：" << anther.vborrower[i];
	}
	return out;
}

istream& operator>>(istream&in, Books&anther)
{

	cout << "请输入书名：" << endl; cin >> anther.name;
	cout << "请输入作者：" << endl; 	cin >> anther.writer;
	//cout << "请输入书籍编号：" << endl;	cin >> anther.id;
	cout << "请输入出版社：" << endl;	cin >> anther.store;
	cout << "请输入本书总数量：" << endl;	cin >> anther.total;
	cout << "请输入出版时间：" << endl;	cin >> anther.pub_time;
	cout << "请输入本书价格：" << endl; cin >> anther.price;
	cout << "请输入出版社：" << endl; cin >> anther.publish;
	if (anther.total - anther.store > 0)
	{
		for (int i = 0; i < anther.total - anther.store; i++)
		{
			cout << "请输入第" << i << "个借书读者：" << endl;
			cin >> anther.vborrower[i];
		}
	}
	return in;
}
