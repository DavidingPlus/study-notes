#include <iostream>


//iostream 提供一个叫命名空间的东西， 标准的命名空间是 std

//方式二：
#if 0
using std::cout; //声明命名空间中的一个变量
using std::endl;
using std::cin;
#endif

//方式三
using namespace std;

int main(void)
{
	int a = 0;
#if 0
	//方式一：
	std::cout << "nihao shijie" << std::endl;
	std::cout << "nihuai shijie" << std::endl;
#endif

	cout << "nihuai shijie" << endl;

	cin >> a;


	return 0;
}