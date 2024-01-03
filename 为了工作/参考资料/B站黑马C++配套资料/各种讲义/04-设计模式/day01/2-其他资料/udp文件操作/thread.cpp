#include <iostream>
#include <vector>
#include<algorithm>
#include<boost/thread.hpp>
#include <windows.h>

using namespace std;
using namespace boost;



void wait(int sec)
{
	boost::this_thread::sleep(boost::posix_time::seconds(sec));
}

void threadA()
{

	for (int i = 0; i < 10;i++)
	{
		wait(1);
		std::cout << i << endl;
	}
}
void threadB()
{
	try
	{
		for (int i = 0; i < 10; i++)
		{
			wait(1);
			std::cout << i << endl;
		}
	}
	catch (boost::thread_interrupted &)
	{
	}
}

void mainO()
{
	boost::thread t(threadA );
	wait(3);
	t.interrupt();//½áÊøÏß³Ì
	t.join();
	cin.get();
}