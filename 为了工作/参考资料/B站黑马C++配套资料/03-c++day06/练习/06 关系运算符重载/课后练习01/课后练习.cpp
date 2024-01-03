#define  _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<cstdio>
using namespace std;

int count(char *str)
{
	int i, num = 0;
	for (i = 0; str[i]; i++)
	{
		if ((str[i]>='a' && str[i]<='z')||(str[i]>='A'&& str[i]<='Z'))
			num++;
	}
	return num;
}
int main()
{
	char text[100];
	cout << "输入一句英语：" << endl;
	gets(text);
	cout << "这个句子里有" << count(text) << "个字母。" << endl;
	system("pause");
	
}


//程序执行结果：
//输入一句英语：
//It is very interesting!
//这个句子里有19个字母。
