#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include<time.h>
#include<sys/timeb.h>

#define MAX 5000000

long getSystemTime(){
	struct timeb tb;
	ftime(&tb);

	return tb.time * 1000 + tb.millitm;
}

//创建数组
int* CreateArray(){
	
	srand((unsigned int)time(NULL));
	int* arr = (int*)malloc(sizeof(int) * MAX);
	for (int i = 0; i < MAX; i++){
		arr[i] = rand() % MAX;
	}

	return arr;
}

//打印
void PrintArray(int arr[], int len){
	for (int i = 0; i < len; i++){
		cout << arr[i] << " ";
	}
	cout << endl;
}

//合并算法 从小到大
void Merge(int arr[], int start, int end, int mid, int* temp){

	int i_start = start;
	int i_end = mid;
	int j_start = mid + 1;
	int j_end = end;

	//表示辅助空间有多少个元素
	int length = 0;

	//合并两个有序序列
	while (i_start <= i_end && j_start <= j_end){

		if(arr[i_start] < arr[j_start]){
			temp[length] = arr[i_start];
			length++;
			i_start++;
		}else{
			temp[length] = arr[j_start];
			j_start++;
			length++;
		}
	}

	//i这个序列
	while (i_start <= i_end){
		temp[length] = arr[i_start];
		i_start++;
		length++;
	}

	//j序列
	while (j_start <= j_end){
		temp[length] = arr[j_start];
		length++;
		j_start++;
	}

	//辅助空间数据覆盖原空间
	for (int i = 0; i < length;i++){
		arr[start + i] = temp[i];
	}

}

//归并排序
void MergeSort(int arr[],int start,int end,int* temp){

	if (start >= end){
		return;
	}

	int mid = (start + end) / 2;
	//分组
	//左半边
	MergeSort(arr,start,mid,temp);
	//右半边
	MergeSort(arr, mid + 1, end,temp);
	//合并
	Merge(arr,start,end,mid,temp);
}

//快速排序 从小到大
void QuickSort(int arr[], int start, int end){

	int i = start;
	int j = end;
	//基准数
	int temp = arr[start];
	if (i < j){

		while (i < j){

			//从右向左去找比基准数小的
			while (i < j && arr[j] >= temp){
				j--;
			}
			//填坑
			if (i < j){
				arr[i] = arr[j];
				i++;
			}
			//从左向右 找比基准数大的数
			while (i < j && arr[i] < temp){
				i++;
			}
			//填坑
			if (i < j){
				arr[j] = arr[i];
				j--;
			}
		}

		//把基准数放到i位置
		arr[i] = temp;
		//对左半部分进行快速排序
		QuickSort(arr, start, i - 1);
		//对右半部分进行快速排序
		QuickSort(arr, i + 1, end);

	}

}

//从小到大
void ShellSort(int arr[], int length){

	int increasement = length;
	int i, j, k;

	do{

		//确定分组的增量
		increasement = increasement / 3 + 1;
		for (i = 0; i < increasement; i++){

			for (j = i + increasement; j < length; j += increasement){

				if (arr[j] < arr[j - increasement]){

					int temp = arr[j];
					for (k = j - increasement; k >= 0 && temp < arr[k]; k -= increasement){
						arr[k + increasement] = arr[k];
					}
					arr[k + increasement] = temp;
				}

			}

		}
	} while (increasement > 1);

}

int main(void){

	int* myArr = CreateArray();
	//PrintArray(myArr,MAX);
	//辅助空间
	int* temp = (int*)malloc(sizeof(int)* MAX);
	long t_start = getSystemTime();
	MergeSort(myArr, 0, MAX - 1, temp);
	//QuickSort(myArr,0,MAX -1);
	//ShellSort(myArr,MAX);
	long t_end = getSystemTime();
	//PrintArray(myArr, MAX);
	printf("排序%d个数据所需时间:%ld\n",MAX,t_end - t_start);

	//释放空间
	free(temp);
	free(myArr);
	return 0;
}