#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
using namespace std;

//创建主席类
//需求 单例模式  为了创建类中的对象，并且保证只有一个对象实例
class ChairMan
{
	//1构造函数 进行私有化
private:
	ChairMan()
	{
		//cout << "创建国家主席" << endl;
	}
	//拷贝构造 私有化
	ChairMan(const ChairMan&c)
	{}

public:
	//提供 get方法 访问 主席
	static ChairMan* getInstance()
	{
		return singleMan;
	}

private:
	static ChairMan * singleMan;

};
ChairMan * ChairMan::singleMan = new ChairMan;

void test01()
{
	/*ChairMan c1;
	ChairMan * c2 = new ChairMan;
	ChairMan * c3 = new ChairMan;*/

	/*ChairMan * cm = ChairMan::singleMan;
	ChairMan * cm2 = ChairMan::singleMan;
*/
	//ChairMan::singleMan = NULL;


	ChairMan * cm1 = ChairMan::getInstance();
	ChairMan * cm2 = ChairMan::getInstance();
	if (cm1 == cm2)
	{
		cout << "cm1 与 cm2相同" << endl;
	}
	else
	{
		cout << "cm1 与 cm2不相同" << endl;
	}

	/*ChairMan * cm3 = new ChairMan(*cm2);
	if (cm3 == cm2)
	{
	cout << "cm3 与 cm2相同" << endl;
	}
	else
	{
	cout << "cm3 与 cm2不相同" << endl;
	}*/


}

int main(){

	//cout << "main调用" << endl; 主席创建先于main调用
	test01();

	system("pause");
	return EXIT_SUCCESS;
}