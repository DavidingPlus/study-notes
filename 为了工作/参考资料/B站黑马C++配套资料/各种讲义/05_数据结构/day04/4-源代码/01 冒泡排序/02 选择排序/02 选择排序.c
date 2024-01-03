#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>

#define MAX 10000

long getSystemTime(){
	struct timeb tb;
	ftime(&tb);
	return tb.time * 1000 + tb.millitm;
}

//交换函数
void Swap(int* a, int* b){
	int temp = *a;
	*a = *b;
	*b = temp;
}

//打印函数
void PrintArray(int arr[], int length){
	for (int i = 0; i < length; i++){
		printf("%d ", arr[i]);
	}
	printf("\n");
}


//冒泡排序改进版
void BubbleSort(int arr[], int length){
	int flag = 0;
	for (int i = 0; i < length && flag == 0; i++){
		flag = 1; //认为已经排序好
		for (int j = length - 1; j > i; j--){
			if (arr[j - 1] < arr[j]){
				flag = 0;
				Swap(&arr[j - 1], &arr[j]);
			}
		}
	}

}


//选择排序
void SelectSort(int arr[],int length){
	
	//选择排序是不是减少交换次数 
	for (int i = 0; i < length;i ++){
		int min = i;
		for (int j = i + 1; j < length; j++){
			if (arr[j] < arr[min]){
				min = j;
			}
		}
		if (min != i){
			Swap(&arr[min],&arr[i]);
		}
	}
}


int main(void){
	

	int arr[MAX];
	int arr2[MAX];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX; i ++){
		int randNum = rand() % MAX;
		arr[i] = randNum;
		arr2[i] = randNum;
	}

	//冒泡排序
	long tbubble_start = getSystemTime();
	BubbleSort(arr, MAX);
	long tbubble_end = getSystemTime();
	printf("冒泡排序%d个元素,所需时间:%ld\n", MAX, tbubble_end - tbubble_start);


	//选择排序
	long tselect_start = getSystemTime();
	SelectSort(arr2, MAX);
	long tselect_end = getSystemTime();
	printf("选择排序%d个元素,所需时间:%ld\n", MAX, tselect_end - tselect_start);

	system("pause");
	return 0;
}