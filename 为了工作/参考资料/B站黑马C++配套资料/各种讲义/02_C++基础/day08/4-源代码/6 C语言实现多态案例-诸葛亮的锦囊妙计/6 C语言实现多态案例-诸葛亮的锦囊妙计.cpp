#define _CRT_SECURE_NO_WARNINGS
#include <iostream>


using namespace std;
//-------------抽象层------------
//定义拆开锦囊方法的类型。
typedef void(TIPS)(void);

//定义锦囊
struct tip
{
	char from[64]; //谁写的
	char to[64];//写给谁的。
	//锦囊的内容
	TIPS *tp;//相当于抽象类的 纯虚函数.
};

//需要一个打开锦囊的架构函数
void open_tips(struct tip *tip_p)
{
	cout << "打开了锦囊" << endl;
	cout << "此锦囊是由" << tip_p->from << "写给 " << tip_p->to << "的。" << endl;
	cout << "内容是" << endl;
	tip_p->tp(); //此时就发生了多态现象。
}

//提供一个创建一个锦囊的方法
struct tip* create_tip(char*from, char *to, TIPS*tp)
{
	struct tip *temp = (struct tip*)malloc(sizeof(struct tip));
	if (temp == NULL) {
		return NULL;
	}
	strcpy(temp->from, from);
	strcpy(temp->to, to);
	//给一个回调函数赋值， 一般称 注册回调函数
	temp->tp = tp;

	return temp;
}

//提供一个销毁锦囊的方法
void destory_tip(struct tip *tp)
{
	if (tp != NULL) {
		free(tp);
		tp = NULL;
	}
}


// ------------- 实现层------------
//诸葛亮写了3个锦囊
void tip1_func(void)
{
	cout << "一到东吴就拜会乔国老" << endl;
}

void tip2_func(void)
{
	cout << "如果主公乐不思蜀，就谎称曹贼来袭。赶紧回来 " << endl;
}

void tip3_func(void)
{
	cout << "如果被孙权追杀，向孙尚香求救" << endl;
}

void tip4_func(void)
{
	cout << "如果求救孙尚香都不灵，  你们去死了， 我是蜀国老大了" << endl;
}


//---------------  业务层-----------------
int main(void)
{
	//创建出3个锦囊
	struct tip *tip1 = create_tip("孔明", "赵云", tip1_func);
	struct tip *tip2 = create_tip("孔明", "赵云", tip2_func);
	struct tip *tip3 = create_tip("孔明", "赵云", tip3_func);
	struct tip *tip4 = create_tip("庞统", "赵云", tip4_func);

	//由赵云进行拆锦囊。
	cout << "刚刚来到东吴， 赵云打开第一个锦囊" << endl;
	open_tips(tip1);
	cout << "-----------" << endl;

	cout << "刘备乐不思蜀， 赵云打开第二个锦囊" << endl;
	open_tips(tip2);
	cout << "-----------" << endl;

	cout << "孙权大军追杀，赵云打开第三个锦囊" << endl;
	open_tips(tip3);
	cout << "-----------" << endl;

	cout << "赵云发现，实在是杀不动了， 打开了第四个锦囊" << endl;
	open_tips(tip4);

	destory_tip(tip1);
	destory_tip(tip2);
	destory_tip(tip3);
	destory_tip(tip4);
	
	return 0;
}