/*
 * server.c
 *
 *  Created on: 2016年3月23日
 *      Author: mengbaoliang
 */

#include "csocket.h"

//初始化
int Init_Socket(int domain,int type,int protocol){

	//初始化socket
	int sock = socket(domain,type,protocol);
	if(sock == -1){
		perror("socket create error:");
		return -1;
	}

	//设置地址复用
	Reuseaddr_Socket(sock);

	//套接字设置非阻塞模式
	Setnonblocking_Socket(sock);

	//绑定ip 端口
	struct sockaddr_in addr;
	addr.sin_family = AF_INET; //IP4因特网域
	addr.sin_port = htons(SERVER_PORT); //设置端口
	addr.sin_addr.s_addr = inet_addr(SERVER_IP); //设置ip
	socklen_t addr_len = sizeof(addr);
	int ret =  bind(sock, (const struct sockaddr *)&addr,addr_len);
	if(ret == -1){
		perror("bind error:");
		goto error;
	}

	//设置套接字接受连接
	ret = listen(sock,SOMAXCONN);
	if(ret == -1){
		perror("listen error:");
		goto error;
	}

	return sock;

error:
	close(sock);
	return -1;
}

//向浏览器发送数据
int SendData_Server(int sock,char* data){
	return send(sock,data,strlen(data),0);
}

//开始监听网络连接
int Accpet_Socket(int sock){

	if(sock == -1){
		perror("Accpet_Server error:");
		return -1;
	}

	struct sockaddr_in clientaddr;
	socklen_t len = sizeof(struct sockaddr);
	int client_sock = accept(sock, (struct sockaddr*)&clientaddr, &len);
	if(client_sock == -1){
		//perror("Accpet_Server error accept:");
		return -1;
	}

	//地址复用
	Reuseaddr_Socket(client_sock);

	//套接字设置非阻塞模式
	//Setnonblocking_Socket(client_sock);

	//printf("\n客户端IP:%s\n",inet_ntoa(clientaddr.sin_addr));
	//printf("客户端端口:%d\n",ntohs(clientaddr.sin_port));

	return client_sock;
}



//receive数据
int Readn_Socket(int sock,void* buf,size_t len){

	size_t nread = len;
	size_t ret;
	char* temp = (char*)buf;
	while(nread > 0){

		if((ret = read(sock,temp,len)) == -1){
			if(errno == EINTR){
				continue;
			}
			else if(errno == EAGAIN){
				continue;
			}
			else{
				return -1;
			}
		}else if(ret == 0){
			break;
		}

		temp += ret;
		nread -= ret;
	}
	return (len - nread);
}
//发送数据
int Writen_Socket(int sock,void* buf,size_t len){

	if(sock == -1 ||  buf == (void*)0){
		perror("Writen_Server error:");
		return -1;
	}

	int writen = len; //发送字节数
	int ret = 0; //已发送字节数
	void* data = buf; //待发送数据

	while(writen > 0){

		if((ret = write(sock,data,len)) <= 0){
			if(errno == EINTR){
				ret = 0;
			}else{
				return -1;
			}
		}

		data += ret;
		writen -= ret;
	}

	return len - writen;
}

//非阻塞
int Setnonblocking_Socket(int sock){

	int ret = fcntl(sock,F_SETFL,O_NONBLOCK);
	if(ret == -1){
		perror("Setnonblocking_Server");
		return -1;
	}

	return 0;
}

//地址复用
int Reuseaddr_Socket(int sock){
	if(sock == -1){
		perror("Reuseaddr_Server error:");
		return -1;
	}
	int opt = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR, (const void *)&opt, sizeof(opt));
	return 0;
}

//关闭连接
int Close_Socket(int sock){

	if(sock == -1){
		perror("Close_Socket error:");
		return -1;
	}

	close(sock);
	return 0;
}


