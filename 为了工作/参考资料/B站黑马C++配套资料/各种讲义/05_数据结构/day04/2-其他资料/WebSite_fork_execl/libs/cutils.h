/*
 * cfile.h
 *
 *  Created on: 2016年3月28日
 *      Author: mengbaoliang
 */

#ifndef LIBS_CUTILS_H_
#define LIBS_CUTILS_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include "cstring.h"
#include "sevdef.h"
#include "chttp.h"

#define BUF_MAX_SIZE 409600 //20M

//文件内容
typedef struct _CFILE{
	long size; //文件大小
	char* strptr; //文件内容
}CFILE;

//读文件
Cstring* ReadFileContent_Utils(char* fileName,HTTP_Info* http);
//遍历文件夹,查找目标文件位置
int IteratorPath_Utils(char* path,char* fileName,char* targetName);
//查询请求文件是否存在，并返回完整资源路径
int IsExistRequestFile_Server(char* filePath,char* targetName);
//解析HTML代码中的C代码，并且通过shell编译 执行生成目标html代码，将html代码替代源文件中的C代码
int ParseCcodeInHtml(char* InBuf,int InBufLen,char* OutBuf);
//生成相应报文
int CreateResponseText(char* filePath,char* txt,long dataLen);
//判断资源文件还是可执行文件
int IsExcuteFile(char* filePaht);




#endif /* LIBS_CUTILS_H_ */
