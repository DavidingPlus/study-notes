#include "Hero.h"

Hero::Hero()
{
	this->m_Hp = 500;

	this->m_Atk = 50;

	this->m_Def = 50;

	this->m_Name = "刘法师";

	this->weapon = NULL;

}

//装备武器
void Hero::EquipWeapon(Weapon * weapon)
{
	this->weapon = weapon;

	cout << "英雄：" << this->m_Name << " 装备了武器 << " << this->weapon->m_WeaponName << " >> " << endl;

}
//攻击
void Hero::Attack(Monster * monster)
{
	int damage = 0;
	int addHp = 0;
	bool isHold = false;
	bool isCrit = false;

	if (this->weapon == NULL) //武器为空 没有加成
	{
		damage = this->m_Atk;
	}
	else
	{
		//基础伤害
		damage = this->m_Atk + this->weapon->getBaseDamage();
		//计算吸血
		addHp = this->weapon->getSuckBlood();
		//计算定身
		isHold = this->weapon->getHold();
		//计算暴击
		isCrit = this->weapon->getCrit();
	}
	if (isCrit) //暴击 伤害 加成
	{
		damage = damage * 2;
		cout << "英雄的武器触发了暴击效果，怪物收到了双倍的伤害，伤害值：" << damage << endl;
	}
	if (isHold)
	{
		cout << "英雄的武器触发了定身效果，怪物停止攻击一回合"<< endl;
	}
	if (addHp > 0)
	{
		cout << "英雄的武器触发了吸血效果，英雄增加的血量为" << addHp << endl;
	}

	//设置怪物定身
	monster->m_Hold = isHold;

	//计算真实伤害
	int trueDamage = (damage - monster->m_Def) > 0 ? damage - monster->m_Def : 1;

	monster->m_Hp -= trueDamage;

	this->m_Hp += addHp;

	cout << "英雄" << this->m_Name << "攻击了敌人 " << monster->m_Name << "造成了 伤害" << trueDamage << endl;
}
