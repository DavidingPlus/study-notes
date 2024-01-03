#ifndef LIBS_CVECTOR_H_
#define LIBS_CVECTOR_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define VECTOR_TRUE 0
#define VECTOR_FALSE 1

typedef void* value_type;
typedef struct _VECTOR{
	value_type* addr;
	int length;
	int capacity;
}Vector;

typedef Vector* varray;
typedef void(*ITERATORARRAY)(value_type);

//初始化数组
varray Init_Vector();
//插入数据
int Insert_Vector(varray vec,int pos,value_type data);
//在尾部插入数据
int PushBack_Vector(varray vec,value_type data);
//获得数组长度
int Size_Vector(varray vec);
//获得数组容量
int Capacity_Vector(varray vec);
//删除指定位置数据
int RemoveByPos_Vector(varray vec,int pos);
//根据值删除
int RemoveByValue_Vector(varray vec,value_type value);
//遍历数据
void IteratorVector(varray vec,ITERATORARRAY iterator);
//清空数组
 void Clear_Vector(varray vec);
//释放空间内存
void Destroy_Vector(varray vec,int flag);

//=========内部接口========
//重新申请空间
int ReSpace_Vector(varray vec);



#endif /* LIBS_CVECTOR_H_ */
