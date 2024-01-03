/*
 * cserver.h
 *
 *  Created on: 2016年3月24日
 *      Author: mengbaoliang
 */

#ifndef LIBS_CSERVER_H_
#define LIBS_CSERVER_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include "chttp.h"
#include "cpool.h"
#include "csocket.h"
#include "sevdef.h"
#include "cutils.h"
#include "cstring.h"

#define BUFF_LEN 1024
#define MAX_PIC_SIZE 409600 //最大图片20M

//记录服务器当前状态
typedef struct _ServerStatus{
	int status; //1运行状态 0停止状态
	time_t starttime; //服务器开始运行时间
}ServerStatus;



//启动服务器
int Statup_Server();
//处理浏览器请求
void DoService_Server(void* arg);
//服务器监听线程
void* DoAccept_Server(void* arg);
//服务器管理线程
void* DoManage_Server(void* arg);
//读取客户端请求
int RecvRequest_Server(int sock,char* buf,size_t buflen);






#endif /* LIBS_CSERVER_H_ */
