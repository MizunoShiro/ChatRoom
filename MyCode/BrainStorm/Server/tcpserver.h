#ifndef TcpServer_H
#define TcpServer_H

#include "thread.h"
#include "tcpsocket.h"
#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

class TcpSocket;
class TcpServer
{
    friend class TcpSocket;
public:
    TcpServer(int threadNum = 20);
    int listen(int port, const char *ip = NULL);
    void start();   //服务器运行

protected:
    //监听回调函数，有客户端连接时会调用这个函数
    static void listenCb(evconnlistener *, evutil_socket_t, sockaddr *,int socklen, void *);
    //监听处理函数
    void listenEvent(evutil_socket_t fd,sockaddr_in *);

    /*虚函数---具体处理客户端事件*/
    //客户端连接
    virtual void connectEvent(TcpSocket *){}
    //客户端可读
    virtual void readEvent(TcpSocket *){}
    //可写
    virtual void writeEvent(TcpSocket *){}
    //关闭
    virtual void closeEvent(TcpSocket *, short ){}
private:
    Thread *m_threadPool;   //线程池
    int m_threadNum;    //线程个数

    event_base *m_base;
    evconnlistener *m_listener; //监听客户端的连接

    int m_nextThread;   //记录下一个线程的下标
};

#endif // TcpServer_H
