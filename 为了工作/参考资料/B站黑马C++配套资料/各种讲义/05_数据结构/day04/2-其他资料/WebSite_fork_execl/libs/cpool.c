/*
 * cpool.c
 *
 *  Created on: 2016年3月25日
 *      Author: mengbaoliang
 */

#include"cpool.h"

//初始化线程池
thread_pool* Init_ThreadPool(){

	thread_pool* pool = (thread_pool*)malloc(sizeof(thread_pool));
	//初始化互斥量
	pthread_mutex_init(&(pool->queue_lock),NULL);
	//初始化条件变量
	pthread_cond_init(&(pool->queue_ready),NULL);
	//初始化任务链表
	pool->tasklist = Init_LinkList();
	//初始化线程池线程数量
	pool->max_thread_num =THREAD_POOL_MAX_NUM;
	//是否销毁线程池
	pool->isdestroy = 0;
	//开辟内存
	pool->threadlist = (pthread_t*)malloc(sizeof(pthread_t)*pool->max_thread_num);
	//创建线程
	int i;
	for(i=0;i<pool->max_thread_num;i++){
		pthread_create(&(pool->threadlist[i]), NULL,Routine_ThreadPool,pool);
	}

	return pool;
}
//线程执行的函数
void* Routine_ThreadPool(void* arg){

	thread_pool* pool = (thread_pool*)arg;

	while(1){

		//加锁
		pthread_mutex_lock(&(pool->queue_lock));

		while(TaskSize_ThreadPool(pool) == 0 && !pool->isdestroy){
			//等待信号，如果有任务了，等待唤醒线程进行工作
			pthread_cond_wait(&(pool->queue_ready),&(pool->queue_lock));
		}
		//如果isdestroy设置为销毁线程
		if(pool->isdestroy){
			pthread_mutex_unlock(&(pool->queue_lock));
			printf("线程池任务线程ID:%d退出!\n",(int)pthread_self());
			pthread_exit(NULL);
		}
		//从任务链表中，获得第一个任务
		Task* task = (Task*)Front_LinkList(pool->tasklist);
		//从任务链表中，删除第一个任务
		PopFront_LinkList(pool->tasklist);
		pthread_mutex_unlock(&(pool->queue_lock));
		//执行任务函数
		task->taskfun(task->arg);
		//销毁任务
		free(task);
		task = NULL;

	}

	return NULL;
}
//线程池添加任务
int AddTask_ThreadPool(thread_pool* pool, void(*taskfun)(void* arg),void* arg){

	if(pool == NULL || taskfun == NULL){
		return -1;
	}

	pthread_mutex_lock(&(pool->queue_lock));
	//创建任务
	Task* task = (Task*)malloc(sizeof(Task));
	task->taskfun = taskfun;
	task->arg = arg;

	//将任务添加到线程池的任务链表中
	PushBack_LinkList(pool->tasklist,(LinkNode*)task);
	pthread_mutex_unlock(&(pool->queue_lock));

	//通知线程，有任务了，可以干活了
	pthread_cond_signal(&(pool->queue_ready));


	return 0;
}

//返回当前任务队列中任务的个数
int TaskSize_ThreadPool(thread_pool* pool){

	if(pool == NULL){
		return -1;
	}

	return Size_LinkList(pool->tasklist);
}

//销毁当前任务链表
int DestroyTaskList_ThreadPool(thread_pool* pool){

	if(pool == NULL){
		return -1;
	}

	printf("开始清空线程池任务列表...\n");
	Destroy_LinkList(pool->tasklist,DestroyCallBack_ThreadPool);
	printf("结束清空线程池任务列表...\n");

	return 0;
}

//销毁线程池
int Destroy_ThreadPool(thread_pool* pool){

	if(pool == NULL){
		return -1;
	}

	if(pool->isdestroy){
		return -1;
	}

	pool->isdestroy = 1;
	//进程等待所有线程退出
	int i=0;
	for(;i<TaskSize_ThreadPool(pool);i++){
		pthread_join(pool->threadlist[i],NULL);
	}
	//唤醒所有线程
	pthread_cond_broadcast(&(pool->queue_ready));
	//销毁任务链表
	DestroyTaskList_ThreadPool(pool);
	//释放线程内存
	free(pool->threadlist);
	//释放互斥量 条件变量
	pthread_mutex_destroy(&(pool->queue_lock));
	pthread_cond_destroy(&(pool->queue_ready));
	//释放线程池
	free(pool);
	pool = NULL;

	printf("线程池释放完毕!\n");

	return 0;
}

//释放内存回调
void DestroyCallBack_ThreadPool(LinkNode* data){
	Task* tast = (Task*)data;
	free(tast);
}


