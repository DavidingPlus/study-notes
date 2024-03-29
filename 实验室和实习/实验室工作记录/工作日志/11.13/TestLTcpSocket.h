/*******************************************************************
 *  Copyright(c) 2022-2025 sinux
 *  All right reserved. See COPYRIGHT for detailed Information
 *
 *  文件名称: TestLTcpSocket.h
 *  简要描述: 
 *  创建日期: 2023/10/11
 *  作者: lich
 *  说明: 测试tcpsocket，封装的服务端类和客户端类，减少代码重复率
 *
 *  修改日期: 
 *  作者: 
 *  说明: 
 ******************************************************************/

#include <gtest/gtest.h>
#include "lhostaddress.h"
#include "lstring.h"
#include "ltcpsocket.h"

using namespace std;

// 创建tcpSocket服务端类
class ServerTcpSocket
{
public:
    LTcpSocket *m_server;
    LString m_msg;
    bool createSever(LAbstractSocket::NetworkProtocol network, LString ip, int port, int max = 5, int buffer = 1024)
    {
        m_server = new LTcpSocket(network);
        m_server->setBufferSize(buffer);
        // 判断绑定结果
        bool bind = m_server->binds(new LHostAddress(ip), port);
        cout << "server_bind: " << bind << endl;
        cout << "server_listen: " << m_server->listens(max) << endl; // 监听socket
        cout << " == start waiting client's connection! ==" << endl;
        return m_server->accepts(); // 阻塞等待接收客户端的连接
        // if (bind)
        // {
        //     // 验证监听结果
        //     cout << "server_listen: " << m_server->listens(max) << endl; // 监听socket
        //     cout << " == start waiting client's connection! ==" << endl;
        //     return m_server->accepts(); // 阻塞等待接收客户端的连接
        // }
        // else
        // {
        //     cout << "server failed to bind!" << endl;
        //     return false;
        // }
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
        else if(rcv == -1)
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
    bool sendMessage(LString msg)
    {
        // 向客户端发送数据
        return m_server->sends(msg);
    }
    bool close()
    {   
        // 关闭客户端的连接
        return m_server->closes(); 
    }
};

// 创建tcpSocket客户端类
class ClientTcpSocket
{
public:
    LTcpSocket *m_client;
    LString m_msg;
    bool createClient(LAbstractSocket::NetworkProtocol network, LString ip, int port, int buffer = 1024)
    {
        m_client = new LTcpSocket(network);
        m_client->setBufferSize(buffer);
        // 判断绑定结果
        bool bind = m_client->binds(new LHostAddress(ip), port);
        cout << "client_bind: " << bind << endl;
        return bind;
    }
    bool connectAndSend(LString serverIp, uint16_t serverPort, LString msg)
    {
        // 连接服务端
        if (m_client->connectToHost(LHostAddress(serverIp), serverPort))
        {
            // 向服务端发送数据
            cout << " == start sending msgs to server! == " << endl;
            m_client->sends(msg);
            return true;
        }
        else
        {
            cout << "client failed to connect to server!" << endl;
            return false;
        }
    }
    LString receiveServer()
    {
        // 接收客户端的数据
        if (m_client->receives() != 0)
        {
            // 获取接收的数据
            m_msg = LString(m_client->pData->bytebuffer);
            cout << "client_recevice_message: " << m_msg << endl;
            return m_msg;
        }
        else
        {
            cout << "client receive null data " << endl;
            return LString();
        }
    }
    bool close()
    {
        return m_client->closes(); // 关闭服务端的连接
    }
};