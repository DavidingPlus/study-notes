/*
 * cmysql.c
 *
 *  Created on: 2016年3月27日
 *      Author: mengbaoliang
 */

#include "cmysql.h"

//初始化
MysqlConn* Init_Mysql(){

	MysqlConn* mysql = (MysqlConn*)malloc(sizeof(MysqlConn));
	//初始化查询结果
	mysql->result = NULL;
	mysql->fieldNums = 0;
	//初始化连接
	mysql_init(&(mysql->conn));
	//连接数据库
	if(mysql_real_connect(&(mysql->conn),MYSQL_HOST,MYSQL_USER,MYSQL_PASSWARD,MYSQL_DATABASE,MYSQL_DPORT,NULL,0) == NULL){
		perror("mysql_real_connect error:");
		return NULL;
	}
	//设置编码
	mysql_query(&(mysql->conn),"SET NAMES UTF8");

	return mysql;
}

//查询结果
int Query_Mysql(MysqlConn* mysql,char* sql){

	//结果集
	MYSQL_ROW row;
	MYSQL_FIELD* field;
	MYSQL_RES* res;

	//如果前一次结果集不为空，那么清空结果集
	if(mysql->result != NULL){
		FreeResult_Mysql(mysql);
	}
	//创建新的数据集
	mysql->result = Init_LinkList();
	//查询
	if(mysql_real_query(&(mysql->conn),sql,strlen(sql)) < 0 ){
		perror("mysql_real_query error:");
		return -1;
	}

	//保存结果集
	if((res = mysql_store_result(&(mysql->conn))) == NULL){
		perror("mysql_store_result error:");
		return -1;
	}

	//获得行结果数量
	mysql->fieldNums = mysql_num_fields(res);
	field = mysql_fetch_fields(res);
	int i;
	while((row = mysql_fetch_row(res))){

		MysqlLine* line = (MysqlLine*)malloc(sizeof(MysqlLine));
		line->list = Init_LinkList();
		//返回当前行中，有多少列
		for(i=0;i<mysql->fieldNums;i++){
			MysqlData* mdata = (MysqlData*)malloc(sizeof(MysqlData));
			mdata->filedName = Init_Cstring(field[i].name);
			mdata->filedValue = Init_Cstring(row[i] ? row[i] : "NULL");
			PushBack_LinkList(line->list,(LinkNode*)mdata);
			//Print_Cstring(mdata->filedName);
			//Print_Cstring(mdata->filedValue);
			//printf("--------------\n");
		}
		PushBack_LinkList(mysql->result,(LinkNode*)line);
	}

	//关闭结果集
	mysql_free_result(res);

	return 0;
}

//根据字段查找回调
int CompareCond_Mysql(LinkNode* data1,void* data2){

	MysqlData* nodedata = (MysqlData*)data1;
	return strcmp(nodedata->filedName->str,(char*)data2) == 0;
}

//根据字段名获得数据
char* GetByField_Mysql(MysqlLine* line,char* fieldName){

	MysqlData* ret =  (MysqlData*)GetByCond_LinkList(line->list,CompareCond_Mysql,fieldName);
	if(ret == NULL){
		return NULL;
	}
	return ret->filedValue->str;
}

//释放单个字段数据
void FreeMysqlData_Mysql(LinkNode* data){

	MysqlData* mydata = (MysqlData*)data;
	Destroy_Cstring(mydata->filedName);
	Destroy_Cstring(mydata->filedValue);
}

//释放单行数据
void FreeSingle_Mysql(LinkNode* data){
	MysqlLine* line = (MysqlLine*)data;
	Iterator_LinkList(line->list,FreeMysqlData_Mysql);

}

//释放结果集
int FreeResult_Mysql(MysqlConn* mysql){

	if(mysql == NULL){
		return -1;
	}

	if(mysql->result == NULL){
		return -1;
	}
	//释放节点内部数据堆区内存
	Iterator_LinkList(mysql->result,FreeSingle_Mysql);
	//释放链表内存
	Destroy_LinkList(mysql->result,DestroyCallBack_Mysql);
	//置空结果集
	mysql->result = NULL;

	return 0;
}
//关闭数据库连接
int Destroy_Mysql(MysqlConn* mysql){
	if(mysql == NULL){
		return -1;
	}
	if(mysql->result != NULL){
		FreeResult_Mysql(mysql);
	}
	mysql_close(&(mysql->conn));
	return 0;
}

//释放内存回调
void DestroyCallBack_Mysql(LinkNode* data){
	MysqlData* mysqldata = (MysqlData*)data;
	free(mysqldata);
}
