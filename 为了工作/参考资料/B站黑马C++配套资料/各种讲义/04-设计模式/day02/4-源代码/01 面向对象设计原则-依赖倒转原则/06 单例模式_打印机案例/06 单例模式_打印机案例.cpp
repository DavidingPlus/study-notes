#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
using namespace std;

class Printer{
private:
	Printer(){
		mCnt = 0;
	}
public:
	static Printer* getInstance(){
		return pPrinter;
	}

	//提供打印接口
	void print(string content){
		cout << "打印内容为:" << content << endl;
		mCnt++;
	}
	int getCnt(){
		return this->mCnt;
	}
private:
	static Printer* pPrinter;
	int mCnt; //统计打印次数
};

Printer* Printer::pPrinter = new Printer;

void test01(){
	
	Printer* printer = Printer::getInstance();
	printer->print("hello world1!");
	printer->print("hello world2!");
	printer->print("hello world3!");

	cout << "打印机打印次数:" << printer->getCnt() << endl;

}

int main(void){
	test01();
	return 0;
}