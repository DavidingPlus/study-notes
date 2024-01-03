#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include<queue>
#include<Windows.h>
using namespace std;


//协议处理类
class HandleClientProtocol{
public:
	//处理增加金币
	void AddMoney(){
		cout << "给玩家增加金币！" << endl;
	}

	//处理增加钻石
	void AddDiamond(){
		cout << "给玩家增加钻石!" << endl;
	}

	//处理玩家装备
	void AddEquipment(){
		cout << "给玩家穿装备！" << endl;
	}

	//处理玩家升级
	void addLevel(){
		cout << "给玩家升级!" << endl;
	}
};

//命令接口
class AbstractCommand{
public:
	virtual void handle() = 0; //处理客户端请求的接口
};


//处理增加金币请求
class AddMoneyCommand :public AbstractCommand{
public:
	AddMoneyCommand(HandleClientProtocol* protocol){
		this->pProtocol = protocol;
	}
	virtual void handle(){
		this->pProtocol->AddMoney();
	}
public:
	HandleClientProtocol* pProtocol;
};

//处理增加钻石的请求
class AddDiamondCommand :public AbstractCommand{
public:
	AddDiamondCommand(HandleClientProtocol* protocol){
		this->pProtocol = protocol;
	}
	virtual void handle(){
		this->pProtocol->AddDiamond();
	}
public:
	HandleClientProtocol* pProtocol;
};


//处理玩家穿装备的请求
class AddEquipmentCommand : public AbstractCommand{
public:
	AddEquipmentCommand(HandleClientProtocol* protocol){
		this->pProtocol = protocol;
	}
	virtual void handle(){
		this->pProtocol->AddEquipment();
	}
public:
	HandleClientProtocol* pProtocol;
};

//处理玩家升级的请求
class AddLevelCommand : public AbstractCommand{
public:
	AddLevelCommand(HandleClientProtocol* protocol){
		this->pProtocol = protocol;
	}
	virtual void handle(){
		this->pProtocol->addLevel();
	}
public:
	HandleClientProtocol* pProtocol;

};


//服务器程序
class Serser{
public:
	void addRequest(AbstractCommand* command){
		mCommands.push(command);
	}

	void startHandle(){
		while (!mCommands.empty()){

			Sleep(2000);
			AbstractCommand* command = mCommands.front();
			command->handle();
			mCommands.pop();
		}
	}
public:
	queue<AbstractCommand*> mCommands;
};

void test01(){
	
	HandleClientProtocol* protocol = new HandleClientProtocol;
	//客户端增加金币的请求
	AbstractCommand* addmoney = new AddMoneyCommand(protocol);
	//客户端增加钻石的请求
	AbstractCommand* adddiamond = new AddDiamondCommand(protocol);
	//客户端穿装备的请求
	AbstractCommand* addequpment = new AddEquipmentCommand(protocol);
	//客户端升级请求
	AbstractCommand* addlevel = new AddLevelCommand(protocol);

	Serser* server = new Serser;
	//将客户端请求加入到处理的队列中
	server->addRequest(addmoney);
	server->addRequest(adddiamond);
	server->addRequest(addequpment);
	server->addRequest(addlevel);

	//服务器开始处理请求
	server->startHandle();
}

int main(void){
	test01();
	return 0;
}