/*
 * clinklist.c
 *
 *  Created on: 2016年3月24日
 *      Author: mengbaoliang
 */

#include "clinklist.h"

//初始化链表
LinkList* Init_LinkList(){

	LinkList* list = (LinkList*)malloc(sizeof(LinkList));
	if(list == NULL){
		printf("Init_LinkList 申请内存失败!\n");
		return NULL;
	}

	list->header.next = NULL;
	list->length = 0;

	return list;
}
//尾部插入节点
int PushBack_LinkList(LinkList* list,LinkNode* node){

	if(list == NULL || node == NULL){
		return -1;
	}

	if(Insert_LinkList(list,list->length,node) == -1){
		return -1;
	}

	return 0;
}
//头部插入节点
int PushFront_LinkList(LinkList* list,LinkNode* node){

	if(list == NULL || node == NULL){
		return -1;
	}

	if(Insert_LinkList(list,0,node) == -1){
		return -1;
	}

	return 0;
}
//指定位置插入
int Insert_LinkList(LinkList* list,int pos,LinkNode* node){

	if(list == NULL || node == NULL || pos < 0){
		return -1;
	}


	if(pos > list->length){
		pos = list->length;
	}


	LinkNode* pCurrent = &(list->header);
	int i;
	for(i=0;i<pos;i++){
		pCurrent = pCurrent->next;
	}

	node->next = pCurrent->next;
	pCurrent->next = node;

	list->length++;

	return 0;
}

//获得指定位置数据
LinkNode* Get_LinkList(LinkList* list,int pos){

	if(pos > list->length - 1){
		return NULL;
	}

	LinkNode* pCurrent = &(list->header);
	int i;
	for(i=0;i<=pos;i++){
		pCurrent = pCurrent->next;
	}

	return pCurrent;
}

//根据条件查找
LinkNode* GetByCond_LinkList(LinkList* list,int(*CONDITION)(LinkNode*,void*),void* data){
	if(list == NULL || CONDITION == NULL){
		return NULL;
	}

	LinkNode* pCurrent = list->header.next;
	while(pCurrent){
		if(CONDITION(pCurrent,data)){
			return pCurrent;
		}
		pCurrent = pCurrent->next;
	}

	return NULL;
}

//获得链表第一个节点
LinkNode* Front_LinkList(LinkList* list){
	return list->header.next;
}
//根据位置删除
int Remove_LinkList(LinkList* list,int pos){

	if(list == NULL || pos < 0 || pos > list->length - 1){
		return -1;
	}

	LinkNode* pCurrent = &(list->header);
	LinkNode* pDel = NULL;
	int i;
	for(i=0;i<pos;i++){
		pCurrent = pCurrent->next;
	}

	pDel = pCurrent->next;
	pCurrent->next = pDel->next;

	list->length--;

	return 0;
}
//头删
int PopFront_LinkList(LinkList* list){

	if(list == NULL){
		return -1;
	}

	if(Remove_LinkList(list,0) == -1){
		return -1;
	}
	return 0;
}
//尾删
int PopBack_LinkList(LinkList* list){

	if(list == NULL){
		return -1;
	}

	if(Remove_LinkList(list,list->length - 1) == -1){
		return -1;
	}
	return 0;
}
//获得链表长度
int Size_LinkList(LinkList* list){

	if(list == NULL){
		return -1;
	}

	return list->length;
}
//遍历链表
void Iterator_LinkList(LinkList* list,void(*IteratorNode)(LinkNode*)){

	if(list == NULL){
		return;
	}

	LinkNode* pCurrent = list->header.next;
	while(pCurrent != (void*)0){
		IteratorNode(pCurrent);
		pCurrent = pCurrent->next;
	}

}
//销毁链表
void Destroy_LinkList(LinkList* list,void(*IteratorNode)(LinkNode*)){

	if(list == NULL){
		return;
	}

	//如果flag为1 就销毁节点内存
	if(IteratorNode != NULL){

		LinkNode* pCurrent = list->header.next;
		LinkNode* pDel = NULL;
		while(pCurrent != NULL){
			printf("释放链表节点内存!\n");
			pDel = pCurrent->next;
			IteratorNode(pCurrent);
			pCurrent =pDel;
		}
	}

	free(list);
}

