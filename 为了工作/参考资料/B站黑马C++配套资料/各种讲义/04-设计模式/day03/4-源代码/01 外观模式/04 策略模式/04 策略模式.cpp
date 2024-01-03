#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;


//抽象武器 武器策略
class WeaponStrategy{
public:
	virtual void UseWeapon() = 0;
};

class Knife : public WeaponStrategy{
public:
	virtual void UseWeapon(){
		cout << "使用匕首！" << endl;
	}
};

class AK47 :public WeaponStrategy{
public:
	virtual void UseWeapon(){
		cout << "使用AK47！" << endl;
	}
};

class Character {
public:
	void setWeapon(WeaponStrategy* weapon){
		this->pWeapon = weapon;
	}
	void ThrowWeapon(){
		this->pWeapon->UseWeapon();
	}
public:
	WeaponStrategy* pWeapon;
};


void test01(){
	
	//创建角色
	Character* character = new Character;

	//武器策略
	WeaponStrategy* knife = new Knife;
	WeaponStrategy* ak47 = new AK47;

	character->setWeapon(knife);
	character->ThrowWeapon();

	character->setWeapon(ak47);
	character->ThrowWeapon();

	delete ak47;
	delete knife;
	delete character;
}

int main(void){

	test01();
	
	return 0;
}