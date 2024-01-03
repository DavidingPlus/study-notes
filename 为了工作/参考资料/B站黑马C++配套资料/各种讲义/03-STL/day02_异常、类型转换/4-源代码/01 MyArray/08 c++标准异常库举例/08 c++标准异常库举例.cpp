#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<stdexcept>
using namespace std;

class Person{
public:
	Person(){
		mAge = 0;
	}
	void setAge(int age){
		if (age < 0 || age > 100){
			throw out_of_range("年龄应该在0-100之间!");
		}
		this->mAge = age;
	}
public:
	int mAge;
};

void test01(){

	Person p;

	try{
		p.setAge(1000);
	}
	catch (exception e){
		cout << e.what() << endl;
	}
}

class MyOutOfRange : public exception{
public:
	MyOutOfRange(char* error){
		pError = new char[strlen(error) + 1];
		strcpy(pError, error);
	}
	~MyOutOfRange(){
		if (pError != NULL){
			delete[] pError;
		}
	}
	virtual const char * what() const{
		return pError;
	};
public:
	char* pError;
};

void fun02(){
	throw MyOutOfRange("我自己的out_of_range!");
}

void test02(){

	try{
		fun02();
	}
	catch (exception& e){
		cout << e.what() << endl;
	}

}


int main(void)
{
	//test01();
	test02();
	return 0;
}