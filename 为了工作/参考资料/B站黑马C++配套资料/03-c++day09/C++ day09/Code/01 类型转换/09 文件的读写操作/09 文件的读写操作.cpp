#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;
//文件读写头文件
#include <fstream>

//写文件
void test01()
{
	//以输出的方式打开文件
	//ofstream ofs("./test.txt", ios::out | ios::trunc);
	//后期指定打开方式
	
	ofstream ofs;
	ofs.open("./test.txt", ios::out | ios::trunc);
	//判断是否打开成功
	if ( !ofs.is_open())
	{
		cout << "打开失败" << endl;
	}

	ofs << "姓名：abc" << endl;
	ofs << "年龄：100" << endl;
	ofs << "性别：男" << endl;


	ofs.close();

}

//读文件
void test02()
{
	ifstream ifs;
	ifs.open("./test.txt", ios::in);

	//判断是否打开成功
	if (!ifs.is_open())
	{
		cout << "打开失败" << endl;
	}

	//第一种方式
	//char buf[1024];

	//while (ifs >>buf) //按行读取
	//{
	//	cout << buf << endl;
	//}

	//第二种方式
	//char buf2[1024];

	//while (!ifs.eof()) //eof读到文件尾
	//{
	//	ifs.getline(buf2, sizeof(buf2));
	//	cout << buf2 << endl;
	//}

	//第三种 不推荐 按单个字符读取
	char c;
	while (  (c = ifs.get() ) != EOF) // EOF文件尾
	{
		cout << c;
	}

	ifs.close();

}



int main(){
	//test01();
	test02();

	system("pause");
	return EXIT_SUCCESS;
}