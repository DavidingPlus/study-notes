#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LinkStack.h"

int IsNumber2(char c){
	return c >= '0' && c <= '9';
}

typedef struct MYNUM{
	LinkNode node;
	int val;
}MyNum;


int Caculate(int left,int right,char c){

	int ret = 0;
	switch (c)
	{
	case '+':
		ret = left + right;
		break;
	case '-':
		ret = left - right;
		break;
	case '*':
		ret = left * right;
		break;
	case '/':
		ret = left / right;
		break;
	default:
		break;
	}

	return ret;
}

int main(void){
	
	//后缀表达式
	char* str = "831-5*+";
	char* p = str;

	//创建栈
	LinkStack* stack = Init_LinkStack();

	while(*p != '\0'){
		//如果是数字 直接入栈
		if (IsNumber2(*p)){
			MyNum* num = (MyNum*)malloc(sizeof(MyNum));
			num->val = *p - '0';
			Push_LinkStack(stack, (LinkNode*)num);
		}
		else{
		
			//先从栈中弹出右操作数
			MyNum* right = (MyNum*)Top_LinkStack(stack);
			int rightNum = right->val;
			Pop_LinkStack(stack);
			free(right);
			//取出左操作数
			MyNum* left = (MyNum*)Top_LinkStack(stack);
			int leftNum = left->val;
			Pop_LinkStack(stack);
			free(left);
			
			int ret = Caculate(leftNum, rightNum,*p);
			//结果入栈
			MyNum* num = (MyNum*)malloc(sizeof(MyNum));
			num->val = ret;
			Push_LinkStack(stack, (LinkNode*)num);
		}
		p++;
	}

	if (Size_LinkStack(stack) == 1){
		MyNum* num =  (MyNum*)Top_LinkStack(stack);
		printf("运算结果是:%d\n",num->val);
		Pop_LinkStack(stack);
		free(num);
	}

	//释放栈
	FreeSpace_LinkStack(stack);

	system("pause");
	return 0;
}