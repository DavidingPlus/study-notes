/*
 * clinklist.h
 *
 *  Created on: 2016年3月24日
 *      Author: mengbaoliang
 */

#ifndef LIBS_CLINKLIST_H_
#define LIBS_CLINKLIST_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

//链表节点
typedef struct _LinkNode{
	struct _LinkNode* next;
}LinkNode;

//链表
typedef struct _LinkList{
	LinkNode header; //头结点
	int length;//链表节点个数
}LinkList;

//初始化链表
LinkList* Init_LinkList();
//尾部插入节点
int PushBack_LinkList(LinkList* list,LinkNode* node);
//头部插入节点
int PushFront_LinkList(LinkList* list,LinkNode* node);
//指定位置插入
int Insert_LinkList(LinkList* list,int pos,LinkNode* node);
//获得指定位置数据
LinkNode* Get_LinkList(LinkList* list,int pos);
//根据条件查找
LinkNode* GetByCond_LinkList(LinkList* list,int(*CONDITION)(LinkNode*,void* data),void* data);
//根据位置删除
int Remove_LinkList(LinkList* list,int pos);
//获得链表第一个节点
LinkNode* Front_LinkList(LinkList* list);
//头删
int PopFront_LinkList(LinkList* list);
//尾删
int PopBack_LinkList(LinkList* list);
//获得链表长度
int Size_LinkList(LinkList* list);
//遍历链表
void Iterator_LinkList(LinkList* list,void(*IteratorNode)(LinkNode*));
//销毁链表
void Destroy_LinkList(LinkList* list,void(*IteratorNode)(LinkNode*));


#endif /* LIBS_CLINKLIST_H_ */
