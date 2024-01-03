#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include<fstream>  //文件读写

//文本文件读写
void test01(){
	
	char* fileName = "C:\\Users\\apple\\Desktop\\source.txt";
	char* TagetName = "C:\\Users\\apple\\Desktop\\target.txt";
	ifstream ism(fileName,ios::in); //只读方式打开文件
	ofstream osm(TagetName,ios::out | ios::app);
	//ifstream ism;
	//ism.open(fileName,ios::in);

	if (!ism){
		cout << "打开文件失败!" << endl;
		return;
	}

	//读文件
	char ch;
	while (ism.get(ch)){
		cout << ch;
		osm.put(ch);
	}

	//关闭文件
	ism.close();
	osm.close();

}

int main(void)
{
	
	test01();
	return 0;
}