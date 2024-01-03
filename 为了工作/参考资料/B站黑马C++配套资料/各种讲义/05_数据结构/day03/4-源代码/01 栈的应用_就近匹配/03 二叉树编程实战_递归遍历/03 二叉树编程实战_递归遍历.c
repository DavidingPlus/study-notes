#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//二叉树结点
typedef struct BINARYNODE{
	char ch;
	struct BINARYNODE* lchild;
	struct BINARYNODE* rchild;
}BinaryNode;

//递归遍历
void Recursion(BinaryNode* root){

	if (root == NULL){
		return;
	}

	//再遍历左子树
	Recursion(root->lchild);
	//再遍历右子树
	Recursion(root->rchild);
	//先访问根结点
	printf("%c", root->ch);
}

void CresteBinaryTree(){ 
	
	//创建结点
	BinaryNode node1 = { 'A', NULL, NULL };
	BinaryNode node2 = { 'B', NULL, NULL };
	BinaryNode node3 = { 'C', NULL, NULL };
	BinaryNode node4 = { 'D', NULL, NULL };
	BinaryNode node5 = { 'E', NULL, NULL };
	BinaryNode node6 = { 'F', NULL, NULL };
	BinaryNode node7 = { 'G', NULL, NULL };
	BinaryNode node8 = { 'H', NULL, NULL };


	//建立结点关系
	node1.lchild = &node2;
	node1.rchild = &node6;
	node2.rchild = &node3;
	node3.lchild = &node4;
	node3.rchild = &node5;
	node6.rchild = &node7;
	node7.lchild = &node8;

	//递归遍历
	Recursion(&node1);
	printf("\n");	
}


int main(void){
	
	CresteBinaryTree();

	system("pause");
	return 0;
}