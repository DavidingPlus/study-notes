#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;


//银行工作人员
class BankWorker{
public:
	void saveService(){
		cout << "办理存款业务..." << endl;
	}
	void payService(){
		cout << "办理支付业务.." << endl;
	}
	void tranferService(){
		cout << "办理转账业务.." << endl;
	}
};

//中层模块
void doSaveBussiness(BankWorker* worker){
	worker->saveService();
}
void doPayBussiness(BankWorker* worker){
	worker->payService();
}
void doTransferBussiness(BankWorker* worker){
	worker->tranferService();
}

void test01(){

	BankWorker* worker = new BankWorker;
	doSaveBussiness(worker); //办理存款业务
	doPayBussiness(worker); //办理支付业务
	doTransferBussiness(worker); //办理转账业务
}



//银行工作人员
class AbstractWorker{
public:
	virtual void doBusiness() = 0;
};

//专门负责办理存款业务的工作人员
class SaveBankWorer :public AbstractWorker{
public:
	virtual void doBusiness(){
		cout << "办理存款业务..." << endl;
	}
};

//专门办理支付业务的工作人员
class PayBankWorker : public AbstractWorker{
public:
	virtual void doBusiness(){
		cout << "办理支付业务..." << endl;
	}
};

//专门办理转账业务的工作人员
class TransferBankWorker : public AbstractWorker{
public:
	virtual void doBusiness(){
		cout << "办理转账业务..." << endl;
	}
};

//中层业务  功能可扩展
void doNewBusiness(AbstractWorker* worker){
	worker->doBusiness();
	delete worker;
}

void tesat02(){
	
	doNewBusiness(new TransferBankWorker);
	doNewBusiness(new PayBankWorker);
	doNewBusiness(new SaveBankWorer);
}





int main(void){
	tesat02();
	return 0;
}