#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;

void PrintArray(int arr[],int len){
	for (int i = 0; i < len;i ++){
		cout << arr[i] << " ";
	}
	cout << endl;
}

void MySwap(int arr[],int a,int b){
	int temp = arr[a];
	arr[a] = arr[b];
	arr[b] = temp;
}
/*
	@param myArr 待调整的数组
	@param index 待调整的结点的下标
	@param len 数组的长度
*/
void HeapAdjust(int arr[], int index, int len){

	//先保存当前结点的下标
	int max = index;
	//保存左右孩子的数组下标
	int lchild = index * 2 + 1;
	int rchild = index * 2 + 2;

	if (lchild < len && arr[lchild] < arr[max]){
		max = lchild;
	}

	if (rchild < len && arr[rchild] < arr[max]){
		max = rchild;
	}

	if (max != index){
		//交换两个结点
		MySwap(arr,max,index);
		HeapAdjust(arr,max,len);
	}
}

//堆排序
void HeapSort(int myArr[], int len){

	//初始化堆
	for (int i = len / 2 - 1; i >= 0; i --){
		HeapAdjust(myArr,i,len);
	}

	//交换堆顶元素和最后一个元素
	for (int i = len - 1; i >= 0; i --){
		MySwap(myArr, 0, i);
		HeapAdjust(myArr, 0, i);
	}

}

int main(void){

	int myArr[] = {4,2,8,0,5,7,1,3,9};
	int len = sizeof(myArr) / sizeof(int);
	PrintArray(myArr, len);
	//堆排序
	HeapSort(myArr,len);
	PrintArray(myArr, len);


	//从数据库读取数据

	for (int i = 0; i < 10;i ++){
		printf("<h1>谁谁今天掉坑里了..</h1>",arr[1]);
	}


	return 0;
}