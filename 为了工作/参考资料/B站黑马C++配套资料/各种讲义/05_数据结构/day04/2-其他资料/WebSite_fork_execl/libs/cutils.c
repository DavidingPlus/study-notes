/*
 * cfile.c
 *
 *  Created on: 2016年3月28日
 *      Author: mengbaoliang
 */

#include "cutils.h"


//生成相应报文
int CreateResponseText(char* filePath,char* txt,long dataLen){



	//截取文件扩展名
	char extension[128] = {0};
	int sslen = strlen(filePath);
	int i =sslen;
	for(;i>=0;i--){
		if(filePath[i]=='.'){
			strncpy(extension,filePath+i+1,sslen-i-1);
		}
	}

	printf("请求文件扩展名:%s\n",extension);

	//状态信息
	char p1[512] = {0};
	sprintf(p1,RESPONSE_STATUS_FORMAT,200);

	//返回数据类型
	char p2[512] = {0};
	if(strcmp(extension,"css") == 0){
		sprintf(p2,RESPONSE_CONTENTTYPE_FORMAT,CSS_CONTENT_TYPE);
	}else if(strcmp(extension,"jpeg") == 0 || strcmp(extension,"jpe") == 0){
		sprintf(p2,RESPONSE_CONTENTTYPE_FORMAT,JPEG_CONTENT_TYPE);
	}else if(strcmp(extension,"jpg") == 0){
		sprintf(p2,RESPONSE_CONTENTTYPE_FORMAT,JPG_CONTENT_TYPE);
	}else if(strcmp(extension,"png") == 0){
		sprintf(p2,RESPONSE_CONTENTTYPE_FORMAT,PNG_CONTENT_TYPE);
	}else{
		sprintf(p2,RESPONSE_CONTENTTYPE_FORMAT,HTML_CONTENT_TYPE);
	}
	//返回数据长度
	char p3[512] = {0};
	sprintf(p3,RESPONSE_CONTENTLENGTH_FORMAT,dataLen);

	strcat(txt,p1);
	strcat(txt,p2);
	strcat(txt,p3);

	return 0;
}

//判断是否是可执行文件
int IsExcuteFile(char* filePath){

	//截取文件扩展名
	char extension[128] = {0};
	int sslen = strlen(filePath);
	int i =sslen;
	for(;i>=0;i--){
		if(filePath[i]=='.'){
			strncpy(extension,filePath+i+1,sslen-i-1);
		}
	}

	if(strcmp(extension,"") == 0){
		return 1;
	}

	return 0;
}


//读文件
Cstring* ReadFileContent_Utils(char* filePath,HTTP_Info* http){


	FILE* fp = fopen(filePath,"rb");
	printf("FILE:%s\n",filePath);
	if(fp == NULL){
		printf("打开文件%s失败!",filePath);
		return NULL;
	}

	//获得文件大小
	fseek(fp,0,SEEK_END);
	long len = ftell(fp);
	fseek(fp,0,SEEK_SET);

	//保存相应报文信息
	char httpinfo[1024] = {0};
	CreateResponseText(filePath,httpinfo,len);

	//保存返回数据
	char buf[BUF_MAX_SIZE] = {0};
	strcat(buf,httpinfo);
	fread(buf + strlen(httpinfo),BUF_MAX_SIZE,1,fp);


	//返回数据保存在Cstring中
	Cstring* htmldata = InitNLength_Cstring(buf,0,len + strlen(httpinfo));

	fclose(fp);

	return htmldata;
}

//遍历文件夹,查找目标文件位置
int IteratorPath_Utils(char* path,char* fileName,char* targetName){

	struct dirent* direc = NULL;
	DIR* dir = NULL;

	if((dir = opendir(path)) != NULL){

		while((direc = readdir(dir)) != NULL){

			if(strcmp(direc->d_name,".") != 0 && strcmp(direc->d_name,"..") != 0 ){
				if(direc->d_type == DT_REG){
					printf("文件:%s\n",direc->d_name);

					if(strcmp(direc->d_name,fileName) == 0){

						getcwd(targetName,1024);
						strcat(targetName,"/www/");
						strcat(targetName,direc->d_name);
						return 0;
					}

				}
				else if(direc->d_type == DT_DIR){
					printf("目录:%s\n",direc->d_name);
					IteratorPath_Utils(direc->d_name,fileName,targetName);
				}
			}

		}

		closedir(dir);
	}

	return -1;
}

//查询请求文件是否存在
int IsExistRequestFile_Server(char* filePath,char* targetName){

	strcat(targetName,ROOT_PATH);
	strcat(targetName,filePath);

	if(access(targetName,0) == -1){
		memset(targetName,0,strlen(targetName)+1);
		strcat(targetName,ERROR_PAGE_PATH);
		strcat(targetName,"/404.html");
		return -1;
	}

	return 0;
}

//解析HTML代码中的C代码，并且通过shell编译 执行生成目标html代码，将html代码替代源文件中的C代码
int ParseCcodeInHtml(char* InBuf,int InBufLen,char* OutBuf){return 0;}

