#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;


void PrintArray(int arr[],int len){
	for (int i = 0; i < len;i++){
		cout << arr[i] << " ";
	}
	cout << endl;
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
			while (i<j && arr[j] >= temp ){
				j--;
			}
			//填坑
			if(i < j){
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
int main(void){


	int myArr[] = {4,2,8,0,5,7,1,3,9};
	int len = sizeof(myArr) / sizeof(int);
	PrintArray(myArr, len);
	QuickSort(myArr,0,len-1);
	PrintArray(myArr, len);

	
	return 0;
}