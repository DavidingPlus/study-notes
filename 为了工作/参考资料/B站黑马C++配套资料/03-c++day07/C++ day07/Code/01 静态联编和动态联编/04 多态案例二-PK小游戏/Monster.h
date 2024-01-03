#pragma once 
#include <iostream>
#include "Weapon.h"
#include <string>
#include "Hero.h"
using namespace std;

class Hero;

class Monster
{
public:
	Monster();

	string m_Name;

	int m_Hp;

	int m_Atk;

	int m_Def;

	bool m_Hold;

	void Attack(Hero * hero);

};