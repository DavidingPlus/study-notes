/*
 * config.c
 *
 *  Created on: 2016年3月20日
 *      Author: mengbaoliang
 */

#include "cconfig.h"

//加载配置文件
varray LoadFile_Config(char* fileName){
	if(fileName == NULL){
		return NULL;
	}
	//初始化动态数组
	varray vec = Init_Vector();
	//打开文件
	FILE* fp = fopen(fileName,"r");
	if(fp == NULL){
		printf("%s文件打开失败!\n",fileName);
		return NULL;
	}
	//读取文件内容
	char buf[MAXLINE];
	memset(buf,0,MAXLINE);

	while(fgets(buf,MAXLINE,fp) != NULL){

		if(buf[0] != NOTESYMBOL){
			int len = strlen(buf) + 1;
			char* pos = strchr(buf,SEPARATOR);
			if(pos != NULL){
				config_item* item = (config_item*)malloc(sizeof(config_item));
				strncpy(item->key , buf , pos-buf);
				strncpy(item->value , pos + 1 , (buf + len) - pos);
				PushBack_Vector(vec,item);
			}

		}

	}

	fclose(fp);
	return vec;
}

//根据key获得value
char* GetByKey_Config(varray vec,char* key){
	if(vec == NULL || key == NULL){
		return NULL;
	}

	int i=0;
	for(;i< vec->length; i++){
		char* infoKey = ((config_item*)vec->addr[i])->key;
		if(strcmp(infoKey,key) == 0){
			return ((config_item*)(vec->addr[i]))->value;
		}
	}
	return NULL;
}

//释放内存
void Destroy_Config(varray vec){
	Destroy_Vector(vec,VECTOR_TRUE);
}

