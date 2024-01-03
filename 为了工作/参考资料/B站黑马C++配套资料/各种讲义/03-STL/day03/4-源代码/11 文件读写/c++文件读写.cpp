#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include<fstream>  //文件读写

//文本文件读写
void test01(){

	char* fileName = "C:\\Users\\apple\\Desktop\\source.txt";
	char* TagetName = "C:\\Users\\apple\\Desktop\\target.txt";
	ifstream ism(fileName, ios::in); //只读方式打开文件
	ofstream osm(TagetName, ios::out | ios::app);
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

//二进制文件操作  对象序列化

class Person{
public:
	Person(){}
	Person(int age,int id):age(age),id(id){}
	void Show(){
		cout << "Age:" << age << " Id:" << id << endl;
	}
public:
	int age;
	int id;
};

void test02(){

	//文本模式读的是文本文件吗？
	//二进制模式读的是二进制文件吗？

#if 0
	Person p1(10, 20), p2(30, 40); //二进制
	//把p1 p2写进文件里	
	ofstream osm(TagetName, ios::out | ios::binary);
	osm.write((char*)&p1, sizeof(Person)); //二进制方式写文件
	osm.write((char*)&p2, sizeof(Person));
	osm.close();
#endif
	char* TagetName = "C:\\Users\\apple\\Desktop\\target.txt";
	ifstream ism(TagetName,ios::in | ios::binary);
	Person p1,p2;
	ism.read((char*)&p1, sizeof(Person)); //从文件读取数据
	ism.read((char*)&p2, sizeof(Person)); //从文件读取数据

	p1.Show();
	p2.Show();
}


int main(void)
{

	//test01();
	test02();
	return 0;
}