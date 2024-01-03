#ifndef SEQSTACK_H
#define SEQSTACK_H

#include<stdlib.h>
#include<stdio.h>

//数组去模拟栈的顺序存储
#define MAX_SIZE 1024
#define SEQSTACK_TRUE 1
#define SEQSTACK_FALSE 0

typedef struct SEQSTACK{
	void* data[MAX_SIZE];
	int size;
}SeqStack;

//初始化栈
SeqStack* Init_SeqStack();
//入栈
void Push_SeqStack(SeqStack* stack,void* data);
//返回栈顶元素
void* Top_SeqStack(SeqStack* stack);
//出栈
void Pop_SeqStack(SeqStack* stack);
//判断是否为空
int IsEmpty(SeqStack* stack);
//返回栈中元素的个数
int Size_SeqStack(SeqStack* stack);
//清空栈
void Clear_SeqStack(SeqStack* stack);
//销毁
void FreeSpace_SeqStack(SeqStack* stack);



#endif