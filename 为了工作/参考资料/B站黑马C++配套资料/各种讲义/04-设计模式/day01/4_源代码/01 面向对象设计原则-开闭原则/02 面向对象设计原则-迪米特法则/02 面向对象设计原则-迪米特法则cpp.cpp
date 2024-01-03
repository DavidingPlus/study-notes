#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
using namespace std;

//迪米特法则 又叫最少知识原则
class AbstractBuilding{
public:
	virtual void sale() = 0;
	virtual string getQuality() = 0;
};

//楼盘A
class BuildingA : public AbstractBuilding{
public:
	BuildingA(){
		mQulity = "高品质";
	}
	virtual void sale(){
		cout << "楼盘A" << mQulity << "被售卖!" << endl;
	}
	virtual string getQuality(){
		return mQulity;
	}
public:
	string mQulity;
};

//楼盘B
class BuildingB : public AbstractBuilding{
public:
	BuildingB(){
		mQulity = "低品质";
	}
	virtual string getQuality(){
		return mQulity;
	}
	virtual void sale(){
		cout << "楼盘B" << mQulity << "被售卖!" << endl;
	}
public:
	string mQulity;
};

//客户端
void test01(){

	BuildingA* ba = new BuildingA;
	if (ba->mQulity == "低品质"){
		ba->sale();
	}

	BuildingB* bb = new BuildingB;
	if (bb->mQulity == "低品质"){
		bb->sale();
	}
}

//中介类
class Mediator{
public:
	Mediator(){
		AbstractBuilding* building = new BuildingA;
		vBuilding.push_back(building);
		building = new BuildingB;
		vBuilding.push_back(building);
	}

	//对外提供接口
	AbstractBuilding* findMyBuilding(string quality){
		for (vector<AbstractBuilding*>::iterator it = vBuilding.begin(); it != vBuilding.end(); it++){
			if ((*it)->getQuality() == quality){
				return *it;
			}
		}

		return NULL;
	}

	~Mediator(){
		for (vector<AbstractBuilding*>::iterator it = vBuilding.begin(); it != vBuilding.end();it++){
			if (*it != NULL){
				delete *it;
			}
		}
	}
public:
	vector<AbstractBuilding*> vBuilding;
};

// 中介 高级中介 中级中介 黑中介
//客户端
void test02(){

	Mediator* mediator = new Mediator;
	AbstractBuilding* building =  mediator->findMyBuilding("高品质");
	if (building != NULL){
		building->sale();
	}
	else{
		cout << "没有符合您条件的楼盘!" << endl;
	}
}


int main(void){

	test01();
	return 0;
}