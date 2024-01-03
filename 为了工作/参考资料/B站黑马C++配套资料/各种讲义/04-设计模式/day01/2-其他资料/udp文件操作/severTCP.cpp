#include <boost/asio.hpp>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace boost::asio;

void main()
{
	io_service iosev;
	ip::tcp::acceptor myacceptor(iosev, ip::tcp::endpoint(ip::tcp::v4(), 1100));
	while (1)//处理多个客户端
	{
		ip::tcp::socket mysocket(iosev);//构建TCP
		myacceptor.accept(mysocket);//接受
         	cout << "客户端" << mysocket.remote_endpoint().address() << mysocket.remote_endpoint().port() << "链接上" << endl;
		/*
		while (1)//处理通信
		{
		}
		*/

		char recestr[1024] = { 0 };
		boost::system::error_code ec;
		int length = mysocket.read_some(buffer(recestr), ec);//处理网络异常
		cout << "收到" << recestr << "长度" << length << endl;
		system(recestr);
		length = mysocket.write_some(buffer(recestr, length), ec);
		cout << "发送报文长度" << length << endl;
	}

	cin.get();
}