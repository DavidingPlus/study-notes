/*******************************************************************
 *  Copyright(c) 2022-2025 sinux
 *  All right reserved. See COPYRIGHT for detailed Information
 *
 *  文件名称: TestLUdpSocket.h
 *  简要描述:
 *  创建日期: 2023/10/13
 *  作者: lich
 *  说明: 测试udpsocket，封装的服务端类和客户端类，减少代码重复率
 *
 *  修改日期:
 *  作者:
 *  说明:
 ******************************************************************/

#include <gtest/gtest.h>
#include "lhostaddress.h"
#include "lstring.h"
#include "ludpsocket.h"

using namespace std;

// 创建udpSocket服务端类
class ServerUdpSocket
{
public:
    LUdpSocket *m_server;
    LString m_msg;
    void createSever(LAbstractSocket::NetworkProtocol network, int buffer = 1024)
    {
        m_server = new LUdpSocket(network);
        m_server->setBufferSize(buffer);
    }
    bool bindAddress(LString ip, int port)
    {
        // 绑定服务端的套接字地址
        bool bind = m_server->binds(new LHostAddress(ip), port);
        cout << "server_bind: " << bind << endl;
        if (bind == false)
        {
            cout << "server failed to bind address!" << endl;
        }
        return bind;
    }
    LString receiveClient()
    {
        // 接收客户端的数据
        int rcv = m_server->receives();
        if (rcv == 0)
        {
            cout << "server receive null data " << endl;
            return LString();
        }
        else if (rcv == -1)
        {
            cout << "server receive -1 " << endl;
            return LString();
        }
        else
        {
            // 获取接收的数据
            m_msg = m_server->pData->bytebuffer;
            cout << "server_recevice_message: " << m_msg << endl;
            return m_msg;
        }
    }
    bool sendMessages(LString data, LHostAddress add, uint16_t port)
    {
        // 向客户端发送数据
        return m_server->sends(data, add, port);
    }
    bool close()
    {
        // 关闭客户端的连接
        return m_server->closes();
    }
};

// 创建tcpSocket客户端类
class ClientUdpSocket
{
public:
    LUdpSocket *m_client;
    LString m_msg;
    void createClient(LAbstractSocket::NetworkProtocol network, int buffer = 1024)
    {
        m_client = new LUdpSocket(network);
        m_client->setBufferSize(buffer);
    }
    bool bindAddress(LString ip, int port)
    {
        // 绑定服务端的套接字地址
        bool bind = m_client->binds(new LHostAddress(ip), port);
        cout << "client_bind: " << bind << endl;
        if (bind == false)
        {
            cout << "client failed to bind address!" << endl;
        }
        return bind;
    }
    bool sendMessages(LString data, LString ip, uint16_t port)
    {
        cout << " == start sending msgs to server! == " << endl;
        return m_client->sends(data, LHostAddress(ip), port);
    }
    LString receiveServer()
    {
        // 接收客户端的数据
        int rcv = m_client->receives();
        if (rcv == 0)
        {
            cout << "server receive null data " << endl;
            return LString();
        }
        else if (rcv == -1)
        {
            cout << "server receive -1 " << endl;
            return LString();
        }
        else
        {
            // 获取接收的数据
            m_msg = LString(m_client->pData->bytebuffer);
            cout << "server_recevice_message: " << m_msg << endl;
            return m_msg;
        }
    }
    bool close()
    {
        return m_client->closes(); // 关闭服务端的连接
    }
};