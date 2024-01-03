#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>
#include <time.h>

#define MAX 100000

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

//从小到大
void ShellSort(int arr[], int length){

	int increasement = length;
	int i, j,k;

	do{

		//确定分组的增量
		increasement = increasement / 3 + 1;
		for (i = 0; i<increasement;i++){
			
			for (j = i + increasement; j < length; j += increasement){		
				
				if (arr[j] < arr[j-increasement] ){
					
					int temp = arr[j];
					for (k = j - increasement; k >= 0 && temp < arr[k];k-=increasement){
						arr[k + increasement] = arr[k];
					}
					arr[k + increasement] = temp;
				}

			}

		}
	} while (increasement > 1);

}

void InsertSort(int arr[], int length){

	int j;
	for (int i = 1; i < length; i++){

		if (arr[i] < arr[i - 1]){

			int temp = arr[i];
			for (j = i - 1; j >= 0 && temp < arr[j]; j--){
				arr[j + 1] = arr[j];
			}
			arr[j + 1] = temp;
		}

	}

}


int main(void){
	
	int arr[MAX];
	int arr2[MAX];
	srand((unsigned int)time(NULL));
	for (int i = 0; i < MAX; i++){
		int randNum = rand() % MAX;
		arr[i] = randNum;
		arr2[i] = randNum;
	}

	//PrintArray(arr, MAX);
	long tshell_start = getSystemTime();
	ShellSort(arr, MAX);
	long tshell_end = getSystemTime();
	printf("希尔排序%d个元素所需时间:%ld\n",MAX,tshell_end - tshell_start);
	//PrintArray(arr, MAX);

	long tinsert_start = getSystemTime();
	InsertSort(arr2, MAX);
	long tinsert_end = getSystemTime();
	printf("插入排序%d个元素所需时间:%ld\n", MAX, tinsert_end - tinsert_start);


	
	system("pause");
	return 0;
}