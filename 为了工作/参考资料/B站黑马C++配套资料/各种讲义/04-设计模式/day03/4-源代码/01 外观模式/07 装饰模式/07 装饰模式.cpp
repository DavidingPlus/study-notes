#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

//一般情况下 用继承实现类的功能拓展
//装饰模式  可以动态给一个类增加功能

//抽象英雄
class AbstractHero{
public:
	virtual void ShowStatus() = 0;
public:
	int mHp;
	int mMp;
	int mAt;
	int mDf;
};

//具体英雄
class HeroA : public AbstractHero{
public:
	HeroA(){
		mHp= 0 ;
		mMp= 0 ;
		mAt= 0 ;
		mDf= 0 ;
	}
	virtual void ShowStatus(){
		cout << "血量:" << mHp << endl;
		cout << "魔法:" << mMp << endl;
		cout << "攻击:" << mAt << endl;
		cout << "防御:" << mDf << endl;
	}
};

//英雄穿上某个装饰物 那么他还是个英雄
class AbstractEquipment : public AbstractHero{
public:
	AbstractEquipment(AbstractHero* hero){
		this->pHero = hero;
	}
	virtual void ShowStatus(){}
public:
	AbstractHero* pHero;
};

//狂徒
class KuangtuEquipment : public AbstractEquipment{
public:
	KuangtuEquipment(AbstractHero* hero) :AbstractEquipment(hero){}
	//增加额外功能
	void AddKuangtu(){
		
		cout << "英雄穿上狂徒之后..." << endl;
		this->mHp = this->pHero->mHp;
		this->mMp = this->pHero->mMp;
		this->mAt = this->pHero->mAt;
		this->mDf = this->pHero->mDf + 30;

		delete this->pHero;

	}
	virtual void ShowStatus(){
		AddKuangtu(); //额外增加功能
		cout << "血量:" << mHp << endl;
		cout << "魔法:" << mMp << endl;
		cout << "攻击:" << mAt << endl;
		cout << "防御:" << mDf << endl;
	}
};

//无尽之刃
class WujinzhiKnife : public AbstractEquipment{
public:
	WujinzhiKnife(AbstractHero* hero) :AbstractEquipment(hero){}
	//增加额外功能
	void AddKuangtu(){

		cout << "英雄装备无尽之刃之后..." << endl;
		this->mHp = this->pHero->mHp;
		this->mMp = this->pHero->mMp;
		this->mAt = this->pHero->mAt + 80;
		this->mDf = this->pHero->mDf;

		delete this->pHero;

	}
	virtual void ShowStatus(){
		AddKuangtu();

		cout << "血量:" << mHp << endl;
		cout << "魔法:" << mMp << endl;
		cout << "攻击:" << mAt << endl;
		cout << "防御:" << mDf << endl;
	}
};

void test01(){
	
	AbstractHero* hero = new HeroA;
	hero->ShowStatus();

	cout << "---------------" << endl;

	//给裸奔的英雄穿上衣服
	hero = new KuangtuEquipment(hero);
	hero->ShowStatus();

	cout << "---------------" << endl;

	//给英雄装备武器
	hero = new WujinzhiKnife(hero);
	hero->ShowStatus();

}


int main(void){
	test01();
	return 0;
}