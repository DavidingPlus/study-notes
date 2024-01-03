/*
 * chttp.h
 *
 *  Created on: 2016年3月28日
 *      Author: mengbaoliang
 */

#ifndef LIBS_CHTTP_H_
#define LIBS_CHTTP_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "clinklist.h"
#include "cstring.h"


//url参数
typedef struct _Request_Param{
	LinkNode node;
	char paramName[128];
	char paramValue[512];
}Request_Param;


//http报文
typedef struct _HTTP_Info{
	char* method;//请求方法
	char* requesturl; //请求URL
	char* requestdata; //请求参数字符串
	LinkList* urlparam; //对请求参数字符串解析，使用链表存储键值对
	LinkList* requestheader; //请求头
	LinkList* postdata; //如果为get请求，则数据为空
}HTTP_Info;


//解析HTTP报文
HTTP_Info* ParseHttp_Server(char* httpText,int len);
//解析请求行
int ParseHttpRequestLine(char* buf,int len,HTTP_Info* http);
//解析请求行请求地址和地址参数
int ParseRequestUrlParam(char* buf,int len,HTTP_Info* http);
//解析=号键值对
int ParseKeyValue(char* buf,int len,LinkList* list);
//解析请求头
int ParseRequestHeader(char* buf,int len,HTTP_Info* http);
//解析数据行
int ParseRequestData(char* buf,int len,HTTP_Info* http);
//根据请求地址参数字符串或者key对应的value
int GetParamValueByKey(char* params,char* key,char* outvalue);
//释放报文内存
int DestroyHTTP_Server(HTTP_Info* http);

//释放内存回调
void DestroyCallBack_Server(LinkNode* data);


#endif /* LIBS_CHTTP_H_ */
