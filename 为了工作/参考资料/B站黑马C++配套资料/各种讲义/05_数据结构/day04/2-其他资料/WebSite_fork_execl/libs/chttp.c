/*
 * chttp.c
 *
 *  Created on: 2016年3月28日
 *      Author: mengbaoliang
 */

#include "chttp.h"

//解析HTTP报文
HTTP_Info* ParseHttp_Server(char* httpText,int len){

	//保存请求数据
	HTTP_Info* http = (HTTP_Info*)malloc(sizeof(HTTP_Info));

	char line[1024] = {0};
	char* textStart = httpText;
	char* textEnd = httpText;
	//请求行
	textEnd = strstr(textEnd,"\r\n");
	strncpy(line,textStart,textEnd-textStart);
	//printf("请求行:%s\n",line);
	//解析请求行

	ParseHttpRequestLine(line,strlen(line),http);
	textEnd += 2;
	textStart = textEnd;

	//请求头部
	http->requestheader = Init_LinkList(); //保存头部信息链表
	while((textEnd = strstr(textEnd,"\r\n")) != NULL){
		if(textEnd-textStart > 0){  //判断是否是空行
			memset(line,0,1024);
			strncpy(line,textStart,textEnd-textStart);
			//创建保存请求头信息链表
			ParseRequestHeader(line,strlen(line)+1,http);
			//printf("请求头:%s\n",line);
		}
		textEnd += 2;
		textStart = textEnd;
	}

	//请求数据
	if(httpText + len - textStart > 0 && *textStart != '\0'){ //判断是否有请求数据
		memset(line,0,1024);
		strncpy(line,textStart,httpText + len - textStart);
		ParseRequestData(line,strlen(line)+1,http);
		//printf("请求数据:%s\n",line);
	}else{
		http->postdata = NULL;
	}


//测试数据
#if 0
	printf("请求方法:\n   %s\n",http->method);
	if(http->urlparam == NULL){
		printf("URL参数为空!\n");
	}else{
		printf("URL参数:\n");
		Iterator_LinkList(http->urlparam,PrintLinkList);
	}
	printf("请求头信息:\n");
	Iterator_LinkList(http->requestheader,PrintLinkList);
	if(http->postdata == NULL){
		printf("POST数据为空!\n");
	}else{
		printf("POST数据:\n");
		Iterator_LinkList(http->postdata,PrintLinkList);
	}

	//释放内存
	DestroyHTTP_Server(http);
#endif



	return http;
}

//释放报文内存
int DestroyHTTP_Server(HTTP_Info* http){

	if(http == NULL){
		printf("报文释放内存失败!\n");
		return -1;
	}

	if(http->method != NULL){
		free(http->method);
		printf("释放请求方法内存...\n");
	}

	if(http->requestdata != NULL){
		free(http->requestdata);
		printf("释放请求地址参数字符串内存...\n");
	}

	if(http->urlparam != NULL){
		printf("开始释放地址参数内存...\n");
		Destroy_LinkList(http->urlparam,DestroyCallBack_Server);
		printf("结束释放地址参数内存...\n");
	}

	if(http->postdata != NULL){
		printf("开始释放POST参数内存...\n");
		Destroy_LinkList(http->postdata,DestroyCallBack_Server);
		printf("结束释放POST参数内存...\n");
	}

	if(http->requesturl != NULL){
		free(http->requesturl);
		printf("释放请求地址内存...\n");
	}

	if(http->requestheader != NULL){
		printf("开始释放请求头内存...\n");
		Destroy_LinkList(http->requestheader,DestroyCallBack_Server);
		printf("结束释放请求头内存...\n");
	}

	free(http);
	printf("释放报文内存结束...\n");

	return 0;
}

//释放内存回调
void DestroyCallBack_Server(LinkNode* data){
	Request_Param* param = (Request_Param*)data;
	free(param);
}

//解析请求行
int ParseHttpRequestLine(char* buf,int len,HTTP_Info* http){

	if(buf == NULL || http== NULL){
		return -1;
	}

	char* lineStart = buf;
	char* linetEnd = buf;
	char lineBuf[1024] = {0};
	int index = 0;
	while((linetEnd = strchr(linetEnd,' ')) != NULL){

		memset(lineBuf,0,1024);

		if(index == 0){ //请求方法
			strncpy(lineBuf,lineStart,linetEnd-lineStart);
			//申请空间
			http->method = (char*)malloc(strlen(lineBuf)+1);
			strncpy(http->method,lineBuf,strlen(lineBuf)+1);
			//printf("请求方法:%s\n",http->method);
			linetEnd += 1;
			lineStart = linetEnd;
		}

		if(index == 1){ //请求URL和参数
			strncpy(lineBuf,lineStart,linetEnd-lineStart);
			//解析URL和参数
			ParseRequestUrlParam(lineBuf,strlen(lineBuf)+1,http);
			break;
		}

		index++;
	}

	return 0;
}


//解析请求行请求地址和地址参数
int ParseRequestUrlParam(char* buf,int len,HTTP_Info* http){

	if(buf == NULL || http== NULL){
		return -1;
	}

	char* lineBuf = buf;

	//判断请求地址是否只有/
	if(strcmp(lineBuf,"/")==0){
		strcat(lineBuf,"index.html");
	}

	//获得URL
	char* urlEnd = strchr(lineBuf,'?');
	if(urlEnd == NULL){ //不存在URL参数

		http->requesturl = (char*)malloc(strlen(lineBuf)+1);
		strncpy(http->requesturl,lineBuf,strlen(lineBuf)+1);
		//请求URL参数不存在
		http->requestdata = NULL;
		http->urlparam = NULL;
	}else{
		char URL[128] = {0};
		strncpy(URL,lineBuf,urlEnd-lineBuf);
		http->requesturl = (char*)malloc(strlen(URL)+1);
		strncpy(http->requesturl,URL,strlen(URL)+1);
		//解析请求URL参数
		char paramBuf[1024] = {0};
		strncpy(paramBuf,urlEnd+1,lineBuf + len - urlEnd-1);
		//保存请求地址参数字符串
		http->requestdata = (char*)malloc(strlen(paramBuf)+2);
		//增加\&转义
		char* addflag =  strchr(paramBuf,'&');
		if(addflag!=NULL){
			strncat(http->requestdata,paramBuf,addflag - 1 - paramBuf);
			strncat(http->requestdata,"\\",1);
			strncat(http->requestdata,addflag,paramBuf + strlen(paramBuf) - addflag);
		}else{
			strncat(http->requestdata,paramBuf,strlen(paramBuf) + 1);
		}
		printf("请求参数PPPPPPPPPP:%s\n",http->requestdata);
		//printf("请求URL参数:%s\n",paramBuf);
		char* paramStart = paramBuf;
		char* paramEnd = paramBuf;

		paramEnd = strchr(paramStart,'&');
		//创建链表
		http->urlparam = Init_LinkList();
		if(paramEnd == NULL){
			ParseKeyValue(paramBuf,strlen(paramBuf) + 1,http->urlparam);

		}else{

			paramEnd = paramStart;
			char kv[256] = {0};
			while((paramEnd = strchr(paramStart,'&')) != NULL){
				strncpy(kv,paramStart,paramEnd-paramStart);
				ParseKeyValue(kv,strlen(kv) + 1,http->urlparam);
				memset(kv,0,256);
				paramEnd+=1;
				paramStart = paramEnd;
			}
			//最后一对参数以\0结尾
			paramEnd = strchr(paramStart,'\0');
			strncpy(kv,paramStart,paramEnd-paramStart);
			ParseKeyValue(kv,strlen(kv) + 1,http->urlparam);
		}
	}

	printf("请求URL是:%s\n",http->requesturl);

	return 0;
}

//根据请求地址参数字符串或者key对应的value
int GetParamValueByKey(char* params,char* key,char* outvalue){

	if(params == NULL || key == NULL){
		return -1;
	}


	char iskey[512] = {0};
	strcat(iskey,key);
	strcat(iskey,"=");

	char* start = strstr(params,iskey);
	//不存在key
	if(start == NULL){
		return -1;
	}

	char* end = strchr(start,'&');
	if(end == NULL){
		end = strchr(start,'\0');
	}

	strncpy(outvalue,start+strlen(iskey),end - start - strlen(iskey));

	return 0;
}

//解析键值对
int ParseKeyValue(char* buf,int len,LinkList* list){

	char* start = buf;
	char* end = buf;
	end = strchr(start,'=');

	Request_Param* param = (Request_Param*)malloc(sizeof(Request_Param));
	memset(param->paramName,0,128);
	memset(param->paramValue,0,512);

	strncpy(param->paramName,start,end-start);
	strncpy(param->paramValue,end + 1,start + len - end -1);

	//将参数插入到参数链表中
	PushBack_LinkList(list,(LinkNode*)param);

	//printf("key:%s value:%s\n",param->paramName,param->paramValue);

	return 0;
}


//解析请求头
int ParseRequestHeader(char* buf,int len,HTTP_Info* http){

	char* lineStart = buf;
	char* lineEnd = buf;

	lineEnd = strchr(lineStart,' ');

	Request_Param* param = (Request_Param*)malloc(sizeof(Request_Param));
	memset(param->paramName,0,128);
	memset(param->paramValue,0,512);

	strncpy(param->paramName,lineStart,lineEnd-lineStart);
	strncpy(param->paramValue,lineEnd+1,lineStart+len-lineEnd-1);

	//将参数插入到参数链表中
	PushBack_LinkList(http->requestheader,(LinkNode*)param);

	//printf("请求头:%s\n",buf);
	//printf("头:key:%s value:%s\n",param->paramName,param->paramValue);


	return 0;
}

//解析数据行
int ParseRequestData(char* buf,int len,HTTP_Info* http){

	http->postdata = Init_LinkList();

	char* dataStart = buf;
	char* dataEnd = buf;

	dataEnd = strchr(dataStart,'&');
	if(dataEnd == NULL){
		ParseKeyValue(buf,strlen(buf)+1,http->postdata);
	}else{

		dataEnd = dataStart;
		char data[1024];
		while((dataEnd = strchr(dataStart,'&')) != NULL){
			memset(data,0,1024);
			strncpy(data,dataStart,dataEnd-dataStart);
			ParseKeyValue(data,strlen(data)+1,http->postdata);
			dataEnd += 1;
			dataStart = dataEnd;
		}

		//解析最后一个post参数
		dataEnd = strchr(dataStart,'\0');
		memset(data,0,1024);
		strncpy(data,dataStart,dataEnd-dataStart);
		ParseKeyValue(data,strlen(data)+1,http->postdata);

	}

	return 0;
}

//打印测试函数
void PrintLinkList(LinkNode* data){

	Request_Param* param = (Request_Param*)data;
	printf("   键值:%s 实值:%s\n",param->paramName,param->paramValue);
}

