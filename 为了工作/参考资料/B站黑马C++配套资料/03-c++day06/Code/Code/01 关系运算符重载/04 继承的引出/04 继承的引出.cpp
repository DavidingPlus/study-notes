#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//class News
//{
//public:
//	void header()
//	{
//		cout << "公共头部" << endl;
//	}
//	void footer()
//	{
//		cout << "公共底部" << endl;
//	}
//	void left()
//	{
//		cout << "左侧列表" << endl;
//	}
//
//	void content()
//	{
//		cout << "新闻播放" << endl;
//	}
//
//};
//
//class YULE
//{
//public:
//	void header()
//	{
//		cout << "公共头部" << endl;
//	}
//	void footer()
//	{
//		cout << "公共底部" << endl;
//	}
//	void left()
//	{
//		cout << "左侧列表" << endl;
//	}
//
//	void content()
//	{
//		cout << "白百合。。。" << endl;
//	}
//
//};
//
//void test01()
//{
//	News news;
//	news.header();
//	news.footer();
//	news.left();
//	news.content();
//
//	//娱乐页
//	YULE yl;
//	yl.header();
//	yl.footer();
//	yl.left();
//	yl.content();
//
//}

//继承写法
//抽象一个 基类的网页  重复的代码都写到这个网页上
class BasePage
{
public:
	void header()
	{
		cout << "公共头部" << endl;
	}
	void footer()
	{
		cout << "公共底部" << endl;
	}
	void left()
	{
		cout << "左侧列表" << endl;
	}
};

class News :public BasePage //继承  News类 继承于 BasePage类
{
public:
	void content()
	{
		cout << "新闻播放" << endl;
	}
};

class YULE :public BasePage
{
public:
	void content()
	{
		cout << "白百合。。。" << endl;
	}
};

class Game :public BasePage
{
public:
	void content()
	{
		cout << "KPL直播" << endl;
	}
};


void test02()
{
	cout << " 新闻网页内容： " << endl;
	News news;
	news.header();
	news.footer();
	news.left();
	news.content();

	cout << " 娱乐网页内容： " << endl;
	YULE yl;
	yl.header();
	yl.footer();
	yl.left();
	yl.content();


	cout << " 游戏网页内容： " << endl;
	Game game;
	game.header();
	game.footer();
	game.left();
	game.content();

}

//继承 减少代码重复内容
//BasePage  基类 (父类)   News 派生类 （子类）



int main(){

	//test01();

	test02();

	system("pause");
	return EXIT_SUCCESS;
}