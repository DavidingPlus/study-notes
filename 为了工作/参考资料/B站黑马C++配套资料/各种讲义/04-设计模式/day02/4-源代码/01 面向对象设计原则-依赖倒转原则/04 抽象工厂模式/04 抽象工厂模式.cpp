#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;


//抽象苹果
class AbstractApple{
public:
	virtual void ShowName() = 0;
};

//中国苹果
class ChinaApple : public AbstractApple{
public:
	virtual void ShowName(){
		cout << "中国苹果!" << endl;
	}
};

//美国苹果
class USAApple : public AbstractApple{
public:
	virtual void ShowName(){
		cout << "美国苹果!" << endl;
	}
};

//倭国苹果
class JapanApple : public AbstractApple{
public:
	virtual void ShowName(){
		cout << "倭国苹果!" << endl;
	}
};

//抽象香蕉
class AbsrtactBanana{
public:
	virtual void ShowName() = 0;
};

//中国香蕉
class ChinaBanana : public AbsrtactBanana{
public:
	virtual void ShowName(){
		cout << "中国香蕉!" << endl;
	}
};

//美国香蕉
class USABanana : public AbsrtactBanana{
public:
	virtual void ShowName(){
		cout << "美国香蕉!" << endl;
	}
};

//倭国香蕉
class JapanBanana : public AbsrtactBanana{
public:
	virtual void ShowName(){
		cout << "倭国香蕉!" << endl;
	}
};

//抽象鸭梨
class AbstractPear{
public:
	virtual void ShowName() = 0;
};

//中国鸭梨
class ChinaPear : public AbstractPear{
public:
	virtual void ShowName(){
		cout << "中国鸭梨!" << endl;
	}
};

//美国鸭梨
class USAPear : public AbstractPear{
public:
	virtual void ShowName(){
		cout << "美国鸭梨!" << endl;
	}
};

//倭国鸭梨
class JapanPear : public AbstractPear{
public:
	virtual void ShowName(){
		cout << "倭国鸭梨!" << endl;
	}
};

//抽象工厂 针对产品族
class AbstracFactory{
public:
	virtual AbstractApple* CreateApple() = 0;
	virtual AbsrtactBanana* CreateBanana() = 0;
	virtual AbstractPear* CreatePear() = 0;
};

//中国工厂
class ChinaFactory : public AbstracFactory{
public:
	virtual AbstractApple* CreateApple(){
		return new ChinaApple;
	}
	virtual AbsrtactBanana* CreateBanana(){
		return new ChinaBanana;
	}
	virtual AbstractPear* CreatePear(){
		return new ChinaPear;
	}
};


//美国工厂
class USAFactory : public AbstracFactory{
public:
	virtual AbstractApple* CreateApple(){
		return new USAApple;
	}
	virtual AbsrtactBanana* CreateBanana(){
		return new USABanana;
	}
	virtual AbstractPear* CreatePear(){
		return new USAPear;
	}
};

//倭国工厂
class JapanFactory : public AbstracFactory{
public:
	virtual AbstractApple* CreateApple(){
		return new JapanApple;
	}
	virtual AbsrtactBanana* CreateBanana(){
		return new JapanBanana;
	}
	virtual AbstractPear* CreatePear(){
		return new JapanPear;
	}
};

void test01(){
		
	AbstracFactory* factory = NULL;
	AbstractApple* apple = NULL;
	AbsrtactBanana* banana = NULL;
	AbstractPear* pear = NULL;

	//中国工厂
	factory = new ChinaFactory;
	apple = factory->CreateApple();
	banana = factory->CreateBanana();
	pear = factory->CreatePear();

	apple->ShowName();
	banana->ShowName();
	pear->ShowName();

	delete pear;
	delete banana;
	delete apple;
	delete factory;
}



int main(void){
	test01();
	return 0;
}