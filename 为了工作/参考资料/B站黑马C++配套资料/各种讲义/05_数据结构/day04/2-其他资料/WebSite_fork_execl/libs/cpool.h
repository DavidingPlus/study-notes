/*
 * cthreadpool.h
 *
 *  Created on: 2016年3月25日
 *      Author: mengbaoliang
 */

#ifndef LIBS_CPOOL_H_
#define LIBS_CPOOL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "clinklist.h"
#include "sevdef.h"

//线程任务
typedef struct _Task{
	LinkNode node;
	void(*taskfun)(void* arg);
	void* arg;
}Task;

//线程池
typedef struct _Cthread_Pool{

	pthread_mutex_t queue_lock; //任务互斥量
	pthread_cond_t queue_ready; //任务条件变量
	LinkList* tasklist; //任务链表
	int max_thread_num; //线程数量
	pthread_t* threadlist; //保存已创建的线程
	int isdestroy; //是否销毁线程池

}thread_pool;

//初始化线程池
thread_pool* Init_ThreadPool();
//线程执行的函数
void* Routine_ThreadPool(void* arg);
//线程池添加任务
int AddTask_ThreadPool(thread_pool* pool,void(*taskfun)(void* arg),void* arg);
//返回当前任务队列中任务的个数
int TaskSize_ThreadPool(thread_pool* pool);
//销毁当前任务链表
int DestroyTaskList_ThreadPool(thread_pool* pool);
//销毁线程池
int Destroy_ThreadPool(thread_pool* pool);
//释放内存回调
void DestroyCallBack_ThreadPool(LinkNode* data);

#endif /* LIBS_CPOOL_H_ */
