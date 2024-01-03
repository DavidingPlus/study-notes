/*
 * vector.c
 *
 *  Created on: 2016年3月19日
 *      Author: mengbaoliang
 */

#include "cvector.h"

//初始化数组
varray Init_Vector(){
	varray vec = (varray)malloc(sizeof(Vector));
	if(vec == NULL){
		return NULL;
	}
	vec->addr = NULL;
	vec->capacity = 0;
	vec->length = 0;

	return vec;
}
//插入数据
int Insert_Vector(varray vec,int pos,value_type data){
	if(vec == NULL || pos < 0 || data == NULL){
		return -1;
	}

	//判断是否有空间,没有空间自动增长空间
	if(ReSpace_Vector(vec) < 0){
		return -2;
	}

	//插入数据
	if(pos > vec->length){
		pos =vec->length;
	}
	vec->addr[pos] = data;
	vec->length++;
	return 0;
}
//在尾部插入数据
int PushBack_Vector(varray vec,value_type data){
	if(vec == NULL || data == NULL){
		return -1;
	}
	//判断是否有空间,没有空间自动增长空间
	if(ReSpace_Vector(vec) < 0){
		return -2;
	}
	//插入数据
	vec->addr[vec->length] = data;
	vec->length++;

	return 0;
}

//获得数组长度
int Size_Vector(varray vec){
	return vec->length;
}
//获得数组容量
int Capacity_Vector(varray vec){
	return vec->capacity;
}
//删除指定位置数据
int RemoveByPos_Vector(varray vec,int pos){
	if(vec == NULL || pos < 0 || pos > vec->length - 1){
		return -1;
	}

	//删除pos位置数据
	int i = 0;
	for(i=pos;i<vec->length-1;i++){
		vec->addr[i] = vec->addr[i+1];
	}

	vec->length--;

	return 0;
}
//根据值删除
int RemoveByValue_Vector(varray vec,value_type value){
	if(vec == NULL || value == NULL){
		return -1;
	}
	//删除第一次出现的位置
	int i =0 , j = 0;
	for(i = 0; i < vec->length; i++){
		if(vec->addr[i] == value){
			for(j = i; j < vec->length;j++){
				vec->addr[j] = vec->addr[j+1];
			}
			vec->length--;
			break;
		}
	}

	return 0;
}
//遍历数据
void IteratorVector(varray vec,ITERATORARRAY iterator){
	if(vec == NULL || iterator == NULL){
		return;
	}
	int i=0;
	for(i=0;i<vec->length;i++){
		iterator(vec->addr[i]);
	}
}
//清空数组
 void Clear_Vector(varray vec){
	 if(vec == NULL){
		 return;
	 }
	 vec->length = 0;
 }
//释放空间内存
void Destroy_Vector(varray vec,int flag){
	if(vec == NULL){
		return;
	}

	//释放用户数据内存
	if(flag == VECTOR_TRUE){
		int i=0;
		for(; i < vec->length ; i ++){
			if(vec->addr[i] != NULL){
				free(vec->addr[i]);
			}
		}
	}

	//释放动态数组内存
	free(vec);
}

//重新申请空间
int ReSpace_Vector(varray vec){

	//判断是否有足够空间
	if(vec->capacity == vec->length){

		//初始化默认的容量20
		if(vec->capacity == 0){
			vec->capacity = 20;
		}else{
			vec->capacity = vec->capacity * 2;
		}
		//申请空间
		value_type* newaddr = (value_type*)malloc(sizeof(value_type) * vec->capacity);
		if(newaddr == NULL){
			return -1;
		}
		//拷贝原空间数据
		memcpy(newaddr,vec->addr,sizeof(value_type)*(vec->length));
		//释放旧空间内存
		free(vec->addr);
		//更新指针指向
		vec->addr = newaddr;
	}

	return 0;
}

