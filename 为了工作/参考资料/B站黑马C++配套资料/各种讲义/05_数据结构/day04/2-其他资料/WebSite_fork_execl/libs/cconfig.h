#ifndef LIBS_CCONFIG_H_
#define LIBS_CCONFIG_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cvector.h"

//每条配置信息
typedef struct _config_item{
	char key[128];
	char value[256];
}config_item;

//注释符号
#define NOTESYMBOL '#'
//配置分隔符
#define SEPARATOR ':'
#define MAXLINE 2048

//加载配置文件
varray LoadFile_Config(char* fileName);
//根据key获得value
char* GetByKey_Config(varray vec,char* key);
//释放内存
void Destroy_Config(varray vec);


#endif /* LIBS_CCONFIG_H_ */
