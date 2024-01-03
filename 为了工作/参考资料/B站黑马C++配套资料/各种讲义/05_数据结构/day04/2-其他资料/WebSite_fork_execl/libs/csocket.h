#ifndef LIBS_CSOCKET_H_
#define LIBS_CSOCKET_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include "sevdef.h"


typedef struct _serverconfig{
	char server_ip[20]; //服务器IP地址
	int server_port; //服务器端口
}serverconfig;

//初始化
int Init_Socket(int domain,int type,int protocol);
//开始监听网络连接
int Accpet_Socket(int sock);
//向浏览器发送数据
int SendData_Server(int sock,char* data);
//receive数据
int Readn_Socket(int sock,void* buf,size_t len);
//发送数据
int Writen_Socket(int sock,void* buf,size_t len);
//非阻塞
int Setnonblocking_Socket(int sock);
//地址复用
int Reuseaddr_Socket(int sock);
//关闭连接
int Close_Socket(int sock);



#endif /* LIBS_CSOCKET_H_ */
