#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

//异常基类
class BaseMyException{
public:
	virtual void  what() = 0;
	virtual ~BaseMyException(){}
};

class TargetSpaceNullException : public BaseMyException{
public:
	virtual void  what(){
		cout << "目标空间空!" << endl;
	}
	~TargetSpaceNullException(){}
};

class SourceSpaceNullException : public BaseMyException{
public:
	virtual void  what(){
		cout << "源空间为空!" << endl;
	}
	~SourceSpaceNullException(){}
};


void copy_str(char* taget,char* source){

	if (taget == NULL){
		throw TargetSpaceNullException();
	}
	if (source == NULL){
		throw SourceSpaceNullException();
	}

	//int len = strlen(source) + 1;
	while (*source != '\0'){
		*taget = *source;
		taget++;
		source++;
	}
}


int main(void){

	char* source = "abcdefg";
	char buf[1024] = {0};
	try{
		copy_str(buf, NULL);
	}
	catch (BaseMyException& ex){
		ex.what();
	}
	
	cout << buf << endl;
	
	return 0;
}