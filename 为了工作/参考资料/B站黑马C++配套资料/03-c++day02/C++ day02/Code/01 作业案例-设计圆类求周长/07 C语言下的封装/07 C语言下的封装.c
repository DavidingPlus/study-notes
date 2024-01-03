#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct Person
{
	char mName[64];

	int mAge;

};

void PersonEat(struct Person *p)
{
	printf("%s 在吃人饭 \n",p->mName);
}

void test01()
{
	struct Person p1;
	strcpy(p1.mName, "德玛西亚");

	PersonEat(&p1);
}


struct Dog
{
	char mName[64];

	int mAge;
};

void DogEat(struct Dog * dog)
{
	printf("%s 在吃狗粮 \n", dog->mName);
}

void test02()
{
	struct Dog d;
	strcpy(d.mName, "旺财");

	DogEat(&d);

	struct Person p1;
	strcpy(p1.mName, "老王");

	DogEat(&p1);

}
//C语言封装 属性和行为分开处理了 ,类型检测不够


int main(){
	//test01();

	test02();
	system("pause");
	return EXIT_SUCCESS;
}