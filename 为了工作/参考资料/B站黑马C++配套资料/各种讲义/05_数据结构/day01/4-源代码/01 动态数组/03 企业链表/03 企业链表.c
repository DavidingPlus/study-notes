#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkList.h"

typedef struct PERSON{
	LinkNode node;
	char name[64];
	int age;
}Person;

void MyPrint(LinkNode* data){
	Person*  p = (Person*)data;
	printf("Name:%s Age:%d\n",p->name,p->age);
}

int MyCompare(LinkNode* node1, LinkNode* node2){
	Person* p1 = (Person*)node1;
	Person* p2 = (Person*)node2;

	if (strcmp(p1->name,p2->name) == 0 && p1->age == p2->age){
		return 0;
	}
	return -1;
}

int main(void){
	
	//创建链表
	LinkList* list = Init_LinkList();

	//创建数据
	Person p1, p2, p3, p4, p5;
	strcpy(p1.name, "aaa");
	strcpy(p2.name, "bbb");
	strcpy(p3.name, "ccc");
	strcpy(p4.name, "ddd");
	strcpy(p5.name, "eee");

	p1.age = 10;
	p2.age = 20;
	p3.age = 30;
	p4.age = 40;
	p5.age = 50;

	//将结点插入链表
	Insert_LinkList(list, 0, (LinkNode*)&p1);
	Insert_LinkList(list, 0, (LinkNode*)&p2);
	Insert_LinkList(list, 0, (LinkNode*)&p3);
	Insert_LinkList(list, 0, (LinkNode*)&p4);
	Insert_LinkList(list, 0, (LinkNode*)&p5);

	//打印
	Print_LinkList(list, MyPrint);

	//删除结点
	Remove_LinkList(list, 2);

	//打印
	printf("---------------\n");
	Print_LinkList(list, MyPrint);

	//查找
	Person findP;
	strcpy(findP.name, "bbb");
	findP.age = 20;
	int pos = Find_LinkList(list, (LinkNode*)&findP, MyCompare);
	printf("位置:%d\n",pos);

	//释放链表内存
	FreeSpace_LinkList(list);


	system("pause");
	return 0;
}