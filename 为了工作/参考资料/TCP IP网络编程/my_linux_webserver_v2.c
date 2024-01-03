
//TCP-IP网络编程 ch24中的服务器详细注释版，增加端口复用
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<pthread.h>

#define BUF_SIZE 1024
#define SMALL_BUF 100

void *request_handler(void *arg);
void send_data(FILE *fp,char *ct,char *file_name);
char *content_type(char *file);
void send_error(FILE *fp);
void error_handing(char *message);

int main(int argc,char *argv[]){

    int serv_sock,clnt_sock;//服务端与客户端的文件描述符
    struct sockaddr_in  serv_adr,clnt_adr;//服务端与客户端总体信息
    int option;
	socklen_t clnt_adr_size,optlen;
    char buf[BUF_SIZE];
    pthread_t  t_id;//子线程
    if(argc != 2){
	    printf("Usage: %s <port>\n",argv[0]);
	    exit(1);
    }

    serv_sock = socket(PF_INET,SOCK_STREAM,0);//指定IPV4互联网协议 以及流通讯方式 返回服务端文件描述符serv_sock
	
	//快递地址再分配，减少Time-wait状态带来的影响
	optlen = sizeof(option);
    option = 1;
    setsockopt(serv_sock, SOL_SOCKET, SO_REUSEADDR, (void *)&option, optlen);
	
	
    memset(&serv_adr,0,sizeof(serv_adr));//初始化服务端地址
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);//0.0.0.0  其实也就是泛指本机，将当前主机IP作为服务器IP
    serv_adr.sin_port = htons(atoi(argv[1]));

	//******************************************
	printf("the serv_sock is %d , address is %s ,and the port is %d\n",serv_sock,inet_ntoa(serv_adr.sin_addr),ntohs(serv_adr.sin_port));
	//*************************************

    if(bind(serv_sock,(struct sockaddr *)&serv_adr,sizeof(serv_adr)) == -1)//绑定服务端IP 与端口号
        error_handing("bind() error");
    if(listen(serv_sock,10) == -1)//最多监听 10 个
        error_handing("listening() error");


    while(1){

        clnt_adr_size = sizeof(clnt_adr);
        clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_adr,&clnt_adr_size);
        printf("New connection request from : %s  and  the port is :%d\n",
		    inet_ntoa(clnt_adr.sin_addr),ntohs(clnt_adr.sin_port));
        
		pthread_create(&t_id,NULL,request_handler,&clnt_sock);//开启一个线程处理新连接
		
		//*************************************
		printf("New thread_id is %ld\n",t_id);
		
        pthread_detach(t_id);//销毁当前线程  ico 这个东西是网页的图标，
		//打开网页的时候会有一次额外的请求来获取ico，所以一次请求会附加一次多阅读额请求来请求图标
		//****************************
		printf("Thread_id %ld has been destoried\n",t_id);
    }

    close(serv_sock);//关闭服务端进程
    return 0;
}

void *request_handler(void *arg){
    int clnt_sock = *((int *)arg);
    char req_line[SMALL_BUF];
    FILE *clnt_read;
    FILE *clnt_write;

    char method[10];
    char ct[10];
    char file_name[30];

    clnt_read = fdopen(clnt_sock,"r");//通过两个FILE指针，分离读写流，
	//好处就是区分读写模式降低实现难度，还有区分I/O缓冲提高缓冲的性能
    clnt_write = fdopen(dup(clnt_sock),"w");//分离前，通过dup函数复制文件描述符
    
	fgets(req_line,SMALL_BUF,clnt_read);
    if(strstr(req_line,"HTTP/") == NULL){// strstr函数是 用于确认后者是不是前者的一个子串，因为是get的请求方式，使用的是HTTP请求
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return ;

    }

    strcpy(method,strtok(req_line," /"));
    strcpy(file_name,strtok(NULL," /"));
    strcpy(ct,content_type(file_name));
	//*********************************************************
	printf("The request is %s , and the request file_name is %s , the ct is %s\n",method,file_name,ct);
	
    if(strcmp(method,"GET") != 0){
        send_error(clnt_write);
        fclose(clnt_read);
        fclose(clnt_write);
        return;
    }
    fclose(clnt_read);
    send_data(clnt_write,ct,file_name);

}


void send_data(FILE*fp,char *ct,char*filename){

    char protocol[] = "HTTP/1.0 200 OK\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[SMALL_BUF];
    char buf[BUF_SIZE];
    FILE *send_file;

    sprintf(cnt_type,"Content-type:%s\r\n\r\n",ct);
    send_file = fopen(filename,"r");
    if(send_file == NULL){
        send_error(fp);
        return;
    }

	printf("准备写入数据:\n protocol:%s. server:%s.cnt_len:%s.cnt_type:%s.filename:%s\n",protocol,server,cnt_len,cnt_type,filename);
    fputs(protocol,fp);
    fputs(server,fp);
    fputs(cnt_len,fp);
    fputs(cnt_type,fp);

    while(fgets(buf,BUF_SIZE,send_file) != NULL){
		fputs(buf,fp);
		fflush(fp);
    }
	
    fflush(fp);
    fclose(fp);
	
	printf("Over\n");
}

char *content_type(char* file){

    char extension[SMALL_BUF];
    char file_name[SMALL_BUF];
    strcpy(file_name,file);
    strtok(file_name,file);
    strtok(file_name,".");
    strcpy(extension,strtok(NULL,"."));

	printf("the extension is %s , and the filename is %s\n",extension,file_name);
	
	
    if(!strcmp(extension,"html") || !strcmp(extension,"htm"))
        return "text/html";
    else
        return "text/plain";

}


void send_error(FILE *fp){
    char protocol[] = "HTTP/1.0 400 Bad Request\r\n";
    char server[] = "Server:Linux Web Server \r\n";
    char cnt_len[] = "Content-length:2048\r\n";
    char cnt_type[] = "Content-type:text/html\r\n\r\n";
    char content[] = "<html><head><title>NETWORK</title></head>"
                     "<body><font size=+5><br>发生错误！ 查看请求文件名和请求方式!"
                     "</font></body></html>";

    fputs(protocol,fp);
    fputs(server,fp);
    fputs(cnt_len,fp);
    fputs(cnt_type,fp);
    fputs(content,fp);

    fflush(fp);
}

void error_handing(char *message){

    fputs(message,stderr);
    fputc('\n',stderr);
    exit(1);
}


















