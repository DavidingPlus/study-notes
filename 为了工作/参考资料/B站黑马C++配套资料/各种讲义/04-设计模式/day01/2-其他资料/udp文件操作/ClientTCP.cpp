#include <boost/asio.hpp>
#include <iostream>
#include <stdlib.h>

using namespace std;
using namespace boost::asio;

void main()
{
	io_service iosev;
	ip::tcp::socket mysorket(iosev);
	ip::tcp::endpoint ep(ip::address_v4::from_string("127.0.0.1"), 1100);

	boost::system::error_code ec;
	mysorket.connect(ep, ec);//链接   自身端口及IP绑定，链接哪一个地址？未显式标明？？？？？？？？？？？？

	while (1)
	{
		char str[1024] = { 0 };
		cout << "请输入";
		cin >> str;
		cout << endl;
		mysorket.write_some(buffer(str), ec);
		memset(str, 0, 1024);//清空字符串
		mysorket.read_some(buffer(str), ec);
		cout << "收到" << str << endl

	}

	cin.get();
}
