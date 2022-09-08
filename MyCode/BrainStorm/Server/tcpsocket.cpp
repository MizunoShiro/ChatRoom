#include "tcpsocket.h"
TcpServer *TcpSocket::m_TcpServer = NULL;
TcpSocket::TcpSocket(TcpServer *server, bufferevent *bev, char *ip, u_int16_t port)
{
    m_TcpServer = server;
    m_bev = bev;
    m_ip = ip;
    m_port = port;
}
char *TcpSocket::getIp()
{
    return m_ip;
}
u_int16_t TcpSocket::getPort()
{
    return m_port;
}

void TcpSocket::readEventCb(bufferevent *, void *ctx)
{
    TcpSocket *s = (TcpSocket *)ctx;
    m_TcpServer->readEvent(s);
}
void TcpSocket::writeEventCb(bufferevent *, void *ctx)
{
    TcpSocket *s = (TcpSocket *)ctx;
    m_TcpServer->writeEvent(s);
}
void TcpSocket::closeEventCb(bufferevent *, short what, void *ctx)
{
    TcpSocket *s = (TcpSocket *)ctx;
    m_TcpServer->closeEvent(s, what);
    delete s;
}
int TcpSocket::readData(void *data, int size)
{
    return bufferevent_read(m_bev, data, size);
}
int TcpSocket::writeData(const void *data,int size)
{
    return bufferevent_write(m_bev, data, size);
}
void TcpSocket::setUserName(std::string name)
{
    _userName = name;
}
std::string TcpSocket::getName()
{
    return _userName;
}
