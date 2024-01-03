/*
 * cmysql.h
 *
 *  Created on: 2016年3月27日
 *      Author: mengbaoliang
 */

#ifndef LIBS_CMYSQL_H_
#define LIBS_CMYSQL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cstring.h"
#include "sevdef.h"
#include "clinklist.h"
#include "include/mysql.h"

//数据库连接
typedef struct _Mysql_Conn{
	MYSQL conn; //数据库连接句柄
	LinkList* result; //查询结果放到链表中
	int fieldNums; //数据结果字段书目
}MysqlConn;

//单行数据
typedef struct _Mysql_Line{
	LinkNode node;
	LinkList* list;
}MysqlLine;

//单列数据
typedef struct _Mysql_Data{
	LinkNode node;
	Cstring* filedName;
	Cstring* filedValue;
}MysqlData;

//初始化
MysqlConn* Init_Mysql();
//查询结果
int Query_Mysql(MysqlConn* mysql,char* sql);
//释放结果集
int FreeResult_Mysql(MysqlConn* mysql);
//根据字段名获得数据
char* GetByField_Mysql(MysqlLine* line,char* fieldName);
//根据字段查找回调
int CompareCond_Mysql(LinkNode* data1,void* data2);
//释放单条数据
void FreeSingle_Mysql(LinkNode*);
//关闭数据库连接
int Destroy_Mysql(MysqlConn* mysql);
//释放内存回调
void DestroyCallBack_Mysql(LinkNode* data);



#endif /* LIBS_CMYSQL_H_ */
