#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include<iomanip>

#if 0 

cout; //全局流对象 输出数据到显示器
cin;
//cerr没有缓冲区 clog有缓冲区
cerr; //标准错误  输出数据到显示器
clog; //标准日志  输出数据到显示器

#endif

//标准输入流 cin.get()
void test01(){

	char ch;
	//while ((ch = cin.get()) != EOF){
	//	cout << ch << endl;
	//}

	//cin.get
	char ch2;
	//cin.get(ch2); //读取一个字符
	char buf[256] = { 0 };
	//cin.get(buf, 256); //从缓冲区读一个字符串
	cin.getline(buf,256); //读取一行数据 不读换行符
	cout << buf;
}

//cin.ignore 忽略当前的字符
void test02(){
	
	char ch;
	cin.get(ch); //从缓冲区要数据 阻塞
	cout << ch << endl;
	cin.ignore(10); //忽略当前字符 从缓冲区取走了
	cin.get(ch);
	cout << ch << endl;
}

void test03(){
	
	cout << "请输入数组或者字符串:" << endl;
	char ch;
	ch = cin.peek(); //偷窥一下缓冲区，返回第一个字符
	if (ch >= '0' && ch <= '9'){
		
		int number;
		cin >> number;
		cout << "您输入的是数字:" << number << endl;

	}
	else{
		char buf[256] = { 0 };
		cin >> buf;
		cout << "您输入的是字符串:" << buf << endl;
	}

}

//cin.putback cin.get
void test04(){
	
	cout << "请输入字符串或者数字:" << endl;
	char ch;
	cin.get(ch); //从缓冲区取走一个字符
	if (ch >= '0' && ch <= '9'){
		
		//ch放回到缓冲区
		cin.putback(ch);
		int number;
		cin >> number;

		cout << "您输入的是数字:" << number << endl;

	}
	else{
	
		cin.putback(ch);
		char buf[256] = { 0 };
		cin >> buf;
		cout << buf << endl;
	}
}

//标准输出流
void test05(){

	cout << "hello world" << endl;
	//cout.flush();
	cout.put('h').put('e').put('l') << endl;
	cout.write("hello Zhaosi!", strlen("hello Zhaosi!"));
}

//格式化输出
void test06(){

	//成员方法的方式
	int number = 10;
	cout << number << endl;
	cout.unsetf(ios::dec); //卸载当前默认的的10进制输出方式
	cout.setf(ios::oct); //八进制输出
	cout.setf(ios::showbase);
	cout << number << endl;
	cout.unsetf(ios::oct); //卸载8进制
	cout.setf(ios::hex);
	cout << number << endl;

	cout.width(10);
	cout.fill('*');
	cout.setf(ios::left);
	cout << number << endl;
	

	//通过控制符
	int number2 = 10;
	cout << hex
		<< setiosflags(ios::showbase)
		<< setw(10)
		<< setfill('*')
		<< setiosflags(ios::left)
		<< number2
		<< endl;

}




int main(void)
{
	//test01();
	test02();
	//test03();
	//test04();
	//test05();
	//test06();
	
	return 0;
}