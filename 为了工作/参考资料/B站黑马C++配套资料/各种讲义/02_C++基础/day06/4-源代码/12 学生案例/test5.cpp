#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

using namespace std;
//第一代学生
class Student1
{
public:
	int strength;

	Student1(int strength)
	{
		this->strength = strength;
	}
	virtual int getStrength()
	{
		return this->strength;
	}
	virtual void print()
	{
		cout << "NO.1 Student1 rolls the ball" << endl;
	}
};
//第二代学生
class Student2 :public Student1
{
public:
	Student2(int strength) :Student1(strength)
	{
	}
	virtual int getStrength()
	{
		return this->strength;
	}
	void print()
	{
		cout << "NO.2 Student rolls the ball" << endl;
	}
};
//传说中的球，，，好重
class Ball
{
public:
	int weight;
	Ball(int weight)
	{
		this->weight = weight;
	}
	int getWeight()
	{
		return this->weight;
	}
	
};
//超级学生
class BugStudent :public Student1
{
public:
	BugStudent(int strength) :Student1(strength)
	{
	}
	virtual int getStrength()
	{
		return this->strength;
	}
	void print()
	{
		cout << "BugStudent rolls the ball" << endl;
	}
};
void rollBalls(Student1 *stre, Ball *we)
{
	if (stre->getStrength() > we->getWeight())
		stre->print();
	else
		cout << "Student can't roll the ball" << endl;
}
int main(void)
{
	Student1 st1(100);
	Student2 st2(300);
	BugStudent Bstu(1000);
	Ball b(500);
	rollBalls(&st1, &b);
	rollBalls(&st2, &b);
	rollBalls(&Bstu, &b);
	system("pause");
	return 0;
}