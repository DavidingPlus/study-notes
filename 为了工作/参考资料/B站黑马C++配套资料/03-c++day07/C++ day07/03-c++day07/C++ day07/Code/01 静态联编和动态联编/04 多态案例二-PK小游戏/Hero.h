#pragma once 
#include <iostream>
#include "Weapon.h"
#include <string>
#include "Monster.h"
using namespace std;

class Monster;

class Hero
{
public:
	Hero();

	string m_Name; //人名

	int m_Atk; //攻击力

	int m_Def; // 防御力

	int m_Hp; //血量

	Weapon * weapon; //武器

	void EquipWeapon(Weapon * weapon);

	void Attack( Monster * monster );
};