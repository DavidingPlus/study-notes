/*
 * cserver.c
 *
 *  Created on: 2016年3月24日
 *      Author: mengbaoliang
 */

#include"cserver.h"

//服务器状态信息
static ServerStatus* serverstatus = NULL;

//启动服务器
int Statup_Server(){

	serverstatus = (ServerStatus*)malloc(sizeof(ServerStatus));
	serverstatus->status = 0;

	//初始化服务器
	int sock = Init_Socket(AF_INET, SOCK_STREAM,0);
	if(sock == -1){
		printf("套接字创建失败!\n");
		return -1;
	}


	pthread_t manageid;

	//开启管理线程
	pthread_create(&manageid,NULL,DoManage_Server,&sock);
	pthread_join(manageid,NULL);


	return 0;
}

//服务器监听线程
void* DoAccept_Server(void* arg){

	int sock = *((int*)arg);

	//初始化线程池
	thread_pool* pool = Init_ThreadPool();
	int client_sock = -1;
	while(1){

		//2毫秒检测一次客户端连接
		usleep(2000);
		if(serverstatus->status == 0){
			//printf("执行了关闭服务线程指令!\n");
			break;
		}

		client_sock = Accpet_Socket(sock);
		if(client_sock == -1){
			//printf("无客户连接....\n");
			continue;
		}

		//解析客户端请求给线程池,线程池处理
		AddTask_ThreadPool(pool,DoService_Server,&client_sock);

	}

	//销毁线程池
	Destroy_ThreadPool(pool);
	//关闭套接字
	close(sock);

	printf("服务器服务线程关闭!\n");
	return NULL;
}

void* DoManage_Server(void* arg){

	pthread_t serviceid;
	int operator = -1;
	char buf[128] = {0};
	while(1){

		printf("请输出您的操作:(0 关闭服务线程 1 启动服务线程)\n");
		fgets(buf,1024,stdin);

		if(strlen(buf) > 2){
			continue;
		}
		if(atoi(buf) < 0 || atoi(buf) > 1){
			continue;
		}
		operator = atoi(buf);
		memset(buf,0,128);
		if(operator == 0){
			if(serverstatus->status == 1){
				serverstatus->status = 0; //设置服务器关闭
				pthread_join(serviceid,NULL);
				break;
			}else{
				printf("服务器未开启!\n");
			}
		}
		else if(operator == 1){
			//开启服务线程
			if(serverstatus->status == 0){
				time(&(serverstatus->starttime));
				printf("%s服务器启动...:\n",ctime(&(serverstatus->starttime)));
				//设置服务器状态，正在运行
				serverstatus->status = 1;
				pthread_create(&serviceid,NULL,DoAccept_Server,arg);
			}else{
				printf("服务器在运行中...\n");
			}

		}

	}

	printf("服务器管理线程关闭!\n");
	return NULL;
}

//读取客户端请求
int RecvRequest_Server(int sock,char* buf,size_t buflen){

	char* pTemp = buf;
	int ret = recv(sock,pTemp,buflen,0);
	if(ret == -1){
		return -1;
	}
	return 0;
}


//处理浏览器请求
void DoService_Server(void* arg){

	int sock = *((int*)arg);
	char buf[BUFF_LEN] = {0};
	RecvRequest_Server(sock,buf,BUFF_LEN);

	printf("客户端请求:\n");
	printf("%s",buf);
	//如果浏览器发送空请求，那么直接断开连接
	if(strcmp(buf,"") == 0){
		printf("请求为空!\n");
		//关闭连接
		close(sock);
		return;
	}

	//解析HTTP报文
	HTTP_Info* http = ParseHttp_Server(buf,strlen(buf) + 1);

	char targetName[1024] = {0};
	IsExistRequestFile_Server(http->requesturl,targetName);
	printf("请求文件路径:%s\n",targetName);

	//如果是可执行程序
	if(IsExcuteFile(targetName)){
		//开子进程执行其他应用程序
		pid_t pid = vfork();
		if(pid < 0){
			perror("fork error:");
		}
		if(pid == 0){
			char sockstr[12] = {0};
			sprintf(sockstr,"%d",sock);
			if (execl(targetName,sockstr,http->requestdata,NULL) < 0 ){
				perror("execl error:");
				printf("执行程序失败!\n");
			}
		}
	}else{
		//读取资源文件 返回
		Cstring* data = ReadFileContent_Utils(targetName,http);
		send(sock,data->str,data->length,0);
		close(sock);
		Destroy_Cstring(data);
	}

	close(sock);
	//释放http内存
	DestroyHTTP_Server(http);
}



