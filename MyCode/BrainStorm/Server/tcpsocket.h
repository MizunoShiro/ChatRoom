#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include "tcpserver.h"
#include <event2/bufferevent.h>
#include <string>

class TcpServer;
class TcpSocket
{
public:
    TcpSocket(TcpServer *sever,bufferevent *bev, char *ip, u_int16_t port);

    //可读事件回调函数
    static void readEventCb(bufferevent *, void *ctx);
    //可写事件回调函数
    static void writeEventCb(bufferevent *, void *ctx);
    //异常事件回调函数
    static void closeEventCb(bufferevent *, short,void *ctx);

    char *getIp();
    u_int16_t getPort();

    void setUserName(std::string name);
    std::string getName();

    //从客户端读数据
    int readData(void *data, int size);
    //往客户端写数据
    int writeData(const void *data,int size);

private:
    static TcpServer *m_TcpServer;   //服务器对象
    bufferevent *m_bev;     //与客户端通信的句柄
    char *m_ip;             //客户端ip
    uint16_t m_port;       //客户端端口

    std::string _userName;
};

#endif // TCPSOCKET_H
