#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

void func01(){
	throw 1; //抛出int类型异常
}

void func02(){
	throw "exception"; //抛出int类型异常
}

class MyException{
public:
	MyException(char* str){
		error = new char[strlen(str)+1];
		strcpy(error, str);
	}
	
	MyException(const MyException& ex){
		this->error = new char[strlen(ex.error) + 1];
		strcpy(this->error,ex.error);
	}
	MyException& operator=(const MyException& ex){
		if (this->error != NULL){
			delete[] this->error;
			this->error = NULL;
		}
		this->error = new char[strlen(ex.error) + 1];
		strcpy(this->error, ex.error);
	}
	

	void what(){
		cout << error << endl;
	}
	~MyException(){
		if (error != NULL){
			delete[] error;
		}
	}
public:
	char* error;
};

void fun03(){

	throw MyException("我刚写异常！");
}


void test01(){
	
	try{
		func01();
	}
	catch (int e){
		cout << "异常捕获！" << endl;
	}
//----------------------------------
	try{
		func02();
	}
	catch (char* e){
		cout << "异常捕获！" << endl;
	}
//----------------------------------
	try{
		fun03();
	}
	catch (MyException e){
		e.what();
	}


}

int main(void){
	
	test01();
	return 0;
}