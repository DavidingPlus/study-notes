/*
 * cstring.h
 *
 *  Created on: 2016年3月26日
 *      Author: mengbaoliang
 */

#ifndef LIBS_CSTRING_H_
#define LIBS_CSTRING_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


//字符串
typedef struct _Cstring{
	char* str;
	int length;
}Cstring;

//初始化字符串
Cstring* Init_Cstring(char* str);
//初始化字符串
Cstring* InitNLength_Cstring(char* str,int start,int len);
//打印字符串
void Print_Cstring(Cstring* string);
//字符串赋值
void Assign_Cstring(Cstring* string,char* str);
//字符串拼接
int Append_Cstring(Cstring* string,char* str);
//获得字符串长度
int Length_Cstring(Cstring* string);
//查找某个字符第一次出现位置
int FindChar_Cstring(Cstring* string,char ch);
//查找某个字符串第一次出现位置
int FindStr_Cstring(Cstring* string,int start,Cstring* str);
//查找某个字符串第一次出现位置
int FindCStr_Cstring(Cstring* string,int start,char* cstr);
//在指定位置插入字符串
int Insert_Cstring(Cstring* string,int pos,Cstring* str);
//字符串替换
int Replace_Cstring(Cstring* string,Cstring* source,Cstring* replacestr);
//字符串替换
int ReplaceByPos_Cstring(Cstring* string,int start,int end,char* replacestr);
//截取字符串
Cstring* SubString_Cstring(Cstring* string,int start,int len);
//删除指定区间字符串
int Delete_Cstring(Cstring* string,int start,int len);
//销毁字符串
void Destroy_Cstring(Cstring* string);




#endif /* LIBS_CSTRING_H_ */
