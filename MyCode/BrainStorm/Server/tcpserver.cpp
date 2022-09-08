#include "tcpserver.h"

TcpServer::TcpServer(int threadNum)
{
    m_nextThread = 0;
    if(threadNum <= 0)
    {
        printf("threadNum can't <= 0!\n");
        exit(1);
    }
    //创建线程池
    m_threadNum = threadNum;
    m_threadPool = new Thread[m_threadNum];
    if(m_threadPool == NULL)
    {
        printf("create threadPool error!\n");
        exit(1);
    }
    m_base = event_base_new();
    if(!m_base)
    {
        printf("Couldn't initialize libevent!\n");
        exit(1);
    }
}
void TcpServer::listenCb(evconnlistener *, evutil_socket_t fd, sockaddr *clientAdd,int, void *data)
{
    TcpServer *p = (TcpServer *)data;
    p->listenEvent(fd, (sockaddr_in *)clientAdd);
}
int TcpServer::listen(int port, const char *ip)
{
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if(ip != NULL)
    {
        inet_aton(ip, &sin.sin_addr);
    }
    m_listener = evconnlistener_new_bind(m_base, listenCb, this,
                                       LEV_OPT_REUSEABLE|LEV_OPT_CLOSE_ON_FREE, -1,
                                       (sockaddr *)&sin, sizeof(sin));
    if(!m_listener)
    {
        printf("Couldn't create a listener!\n");
        return -1;
    }
    //开启线程池
    for(int i = 0; i < m_threadNum; i++)
    {
        m_threadPool[i].start();
        //printf("线程%d启动\n",i+1);
    }
    return 0;
}

void TcpServer::listenEvent(evutil_socket_t fd,sockaddr_in *clientAdd)
{
    char *ip = inet_ntoa(clientAdd->sin_addr);      //客户端ip
    uint16_t port = ntohs(clientAdd->sin_port);     //客户端端口
    //从线程池中选择一个线程处理客户端的请求
    //以轮询的方式选择线程
    event_base *base = m_threadPool[m_nextThread].getBase();
    m_nextThread = (m_nextThread + 1) % m_threadNum;
    bufferevent *bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if(!bev)
    {
        printf("Error constructing bufferevent!\n");
        event_base_loopbreak(base);
        return;
    }

    //创建一个通信对象
    TcpSocket *s = new TcpSocket(this, bev, ip, port);
    bufferevent_setcb(bev, s->readEventCb, s->writeEventCb, s->closeEventCb, s);
    bufferevent_enable(bev, EV_READ);
    bufferevent_enable(bev, EV_WRITE);
    bufferevent_enable(bev, EV_SIGNAL);
    //开启连接事件
    connectEvent(s);
}
void TcpServer::start()
{
    event_base_dispatch(m_base);
    evconnlistener_free(m_listener);
    event_base_free(m_base);

    printf("done\n");
}
