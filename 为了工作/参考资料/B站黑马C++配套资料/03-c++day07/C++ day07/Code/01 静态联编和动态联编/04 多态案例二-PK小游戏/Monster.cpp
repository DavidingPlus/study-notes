#include "Monster.h"


Monster::Monster()
{
	this->m_Hp = 300;

	this->m_Atk = 70;

	this->m_Def = 40;

	this->m_Hold = false;

	this->m_Name = "比克大魔王";
}

void Monster::Attack(Hero * hero)
{
	if (this->m_Hold)
	{
		cout << "怪物" << this->m_Name << "被定身了，本回合无法攻击" << endl;
		return;
	}

	//计算攻击伤害
	int damage = (this->m_Atk - hero->m_Def) > 0 ? this->m_Atk - hero->m_Def : 1;

	hero->m_Hp -= damage;

	cout << "怪物" << this->m_Name << "攻击了英雄 " << hero->m_Name << "造成了 伤害" << damage << endl;

}
