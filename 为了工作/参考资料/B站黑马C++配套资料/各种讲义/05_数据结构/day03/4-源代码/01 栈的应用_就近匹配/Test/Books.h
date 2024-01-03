#pragma once
#include <string>
#include <vector>
#include "Readers.h"
using namespace std;

class Books{

	friend ostream& operator<<(ostream&out, Books&anther);
	friend istream& operator>>(istream&in, Books&anther);

public:
	Books();			  //book类构造函数
	void set_id();		//设置书号
	string get_name();           //获取书名
	string get_writer();		  //获取作者
	vector<Reader>& get_vborrower();			  //获取借书人
	int get_id();				 //获取书号
	int get_borr_num();			 //获取借书人学号
	int get_store();			 //获取现存量
	int get_total();			 // 获取总存量
	string get_pub_time();		 //获取出版时间
	float get_price();			//获取价格
	string get_publish();		 //获取出版社
private:
	string name;        //书名
	string writer;      //作者
	vector<Reader> vborrower;//借书人
	int id;            //书号
	int store;         //现存量
	int total;         // 总存量
	string pub_time;   //出版时间
	float price;       //价格
	string publish;    //出版社

};