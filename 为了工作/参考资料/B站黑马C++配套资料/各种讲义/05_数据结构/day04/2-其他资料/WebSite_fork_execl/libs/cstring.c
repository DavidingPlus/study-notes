/*
 * cstring.c
 *
 *  Created on: 2016年3月26日
 *      Author: mengbaoliang
 */

#include "cstring.h"


//初始化字符串
Cstring* Init_Cstring(char* str){

	Cstring* string = (Cstring*)malloc(sizeof(Cstring));
	if(str == NULL){
		string->str = NULL;
		string->length = 0;
		return string;
	}

	string->str = (char*)malloc(strlen(str)+1);
	strcpy(string->str,str);
	string->length = strlen(string->str);

	return string;
}

//初始化字符串
Cstring* InitNLength_Cstring(char* str,int start,int len){

	if(str == NULL){
		return Init_Cstring(NULL);
	}

	Cstring* string = (Cstring*)malloc(sizeof(Cstring));
	string->str = (char*)malloc(len);
	memset(string->str,0,len);

	int i =0;
	for(;i<len;i++){
		string->str[i] = str[i];
	}

	string->length = len;

	return string;
}

//字符串赋值
void Assign_Cstring(Cstring* string,char* str){

	if(string->str != NULL){
		free(string->str);
		string->str = NULL;
	}

	string->str = (char*)malloc(strlen(str)+1);
	strcpy(string->str,str);
	string->length = strlen(string->str);
	string->length = strlen(str);
}

//字符串拼接
int Append_Cstring(Cstring* string,char* str){

	if(string == NULL || str == NULL){
		return -1;
	}

	if(string->str == NULL){
		Assign_Cstring(string,str);
		return 0;
	}

	int len = strlen(string->str) + strlen(str) + 1;
	char* newspace = (char*)malloc(len);
	strcat(newspace,string->str);
	strcat(newspace,str);

	free(string->str);
	string->str = newspace;
	string->length = len - 1;

	return 0;
}
//获得字符串长度
int Length_Cstring(Cstring* string){
	return string->length;
}
//查找某个字符第一次出现位置
int FindChar_Cstring(Cstring* string,char ch){
	char* pos =strchr(string->str,ch);
	if(pos == NULL){
		return -1;
	}
	return pos - string->str;
}
//查找某个字符串第一次出现位置
int FindStr_Cstring(Cstring* string,int start,Cstring* str){
	if(start > string->length){
		start = string->length;
	}
	char* pos = strstr(string->str + start,str->str);
	if(pos == NULL){
		return -1;
	}
	return pos - string->str;
}

//查找某个字符串第一次出现位置
int FindCStr_Cstring(Cstring* string,int start,char* cstr){
	if(start > string->length){
		start = string->length;
	}
	char* pos = strstr(string->str + start,cstr);
	if(pos == NULL){
		return -1;
	}
	return pos - string->str;
}

//在指定位置插入字符串
int Insert_Cstring(Cstring* string,int pos,Cstring* str){

	if(pos > string->length){
		pos = string->length;
	}


	char* start1 = string->str;
	char* end1 = string->str + pos;
	char* start2 = end1;
	char* end2 = string->str + string->length;


	int len = string->length + strlen(str->str) + 1;
	char* newspace = (char*)malloc(len);
	strncat(newspace,start1,end1-start1);
	strncat(newspace,str->str,str->length);
	strncat(newspace,start2,end2-start2);

	free(string->str);

	string->str = newspace;
	string->length = len - 1;


	return 0;
}

//截取字符串
Cstring* SubString_Cstring(Cstring* string,int start,int end){

	if(start > string->length){
		start = string->length;
	}

	if(start + end > string->length){
		end = string->length - start;
	}

	int len = end;
 	char* newspace =  (char*)malloc(len);
 	memset(newspace,0,len);
 	strncpy(newspace,string->str + start,len);

 	Cstring* newstr =  Init_Cstring(NULL);
 	newstr->str = newspace;
 	newstr->length = len;

 	return newstr;

}

//删除指定区间字符串
int Delete_Cstring(Cstring* string,int first,int len){

	if(first > string->length){
		first = string->length;
	}

	if(first + len > string->length){
		len = string->length - first;
	}
	char* start1 = string->str;
	char* end1 =  string->str + first;
	char* start2 = string->str + first + len;
	char* end2 = string->str +  string->length;

	int size = (end1-start1) + (end2-start2) + 1;
	char* newspace = (char*)malloc(size);
	strncat(newspace,start1,end1-start1);
	strncat(newspace,start2,end2-start2);

	free(string->str);

	string->str = newspace;
	string->length = len - 1;

	return 0;
}

//字符串替换
int Replace_Cstring(Cstring* string,Cstring* source,Cstring* replacestr){

	if(string == NULL || source == NULL || replacestr == NULL){
		return -1;
	}

	int pos = FindStr_Cstring(string,0,source);
	if(pos == -1){
		return -1;
	}

	char* start1 = string->str;
	char* end1 = string->str + pos;
	char* start2 = string->str + pos + source->length;
	char* end2 = string->str + string->length;

	int len = string->length + replacestr->length + 1;
	char* newspace = (char*)malloc(len);
	strncat(newspace,start1,end1-start1);
	strncat(newspace,replacestr->str,replacestr->length);
	strncat(newspace,start2,end2-start2);


	free(string->str);
	string->str = newspace;
	string->length = len - 1;

	return 0;
}

//字符串替换
int ReplaceByPos_Cstring(Cstring* string,int start,int end,char* replacestr){

	if(string == NULL || replacestr == NULL){
		return -1;
	}

	if(start > string->length){
		start = string->length;
	}

	if(start + end > string->length){
		end = string->length - start;
	}



	char* start1 = string->str;
	char* end1 = string->str + start;
	char* start2 = string->str + start + end;
	char* end2 = string->str + string->length;

	int len = (end1 - start1) + (end2 - start2) + strlen(replacestr) + 1;

	char* newspace = (char*)malloc(len);
	strncat(newspace,start1,end1-start1);
	strncat(newspace,replacestr,strlen(replacestr) + 1);
	strncat(newspace,start2,end2-start2);

	free(string->str);
	string->str = newspace;
	string->length = len - 1;

	return 0;
}

//销毁字符串
void Destroy_Cstring(Cstring* string){
	if(string != NULL){
		free(string->str);
		string->length = 0;
	}
}
//打印字符串
void Print_Cstring(Cstring* string){
	printf("%s\n",string->str);
}

